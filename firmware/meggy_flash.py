#!/usr/bin/env python3
"""
meggy_flash.py — PC-side flash programmer for the Meggy ROM board.
Communicates with the AT90USB1286 firmware over USB (WinUSB via Zadig,
or native HID/libusb on Linux/macOS).

Requirements:
    pip install pyusb
    Windows: install WinUSB driver via Zadig for "Meggy Flash Programmer"
    Linux:   may need  sudo apt install libusb-1.0-0
             and a udev rule — see bottom of this file

Usage examples:
    python meggy_flash.py --identify
    python meggy_flash.py --read dump.bin
    python meggy_flash.py --write kickstart.bin --swap --image 0
    python meggy_flash.py --write diagrom.bin   --swap --image 1
    python meggy_flash.py --verify kickstart.bin --swap --image 0
    python meggy_flash.py --erase
"""

import argparse
import os
import sys
import time

try:
    import usb.core
    import usb.util
except ImportError:
    print("ERROR: pyusb not found.  Install with:  pip install pyusb")
    print("       Windows: also install WinUSB driver via Zadig.")
    sys.exit(1)

# ---------------------------------------------------------------------------
# USB device identity — must match Descriptors.c
# ---------------------------------------------------------------------------
USB_VID = 0x03EB
USB_PID = 0x2044

# Endpoint addresses — must match Descriptors.c
EP_TX   = 0x83   # IN  endpoint (device → host), address 3
EP_RX   = 0x04   # OUT endpoint (host → device), address 4
EP_SIZE = 64     # max packet size

# ---------------------------------------------------------------------------
# Protocol constants — must match firmware protocol.h
# ---------------------------------------------------------------------------
MAGIC            = b'\x4D\x47'

CMD_IDENTIFY     = 0x01
CMD_READ         = 0x02
CMD_CHIP_ERASE   = 0x03
CMD_SECTOR_ERASE = 0x04
CMD_PROGRAM      = 0x05
CMD_GET_STATUS   = 0x06
CMD_FLASH_RESET  = 0x07
CMD_AVR_RESET    = 0x08

STATUS_OK        = 0x00
STATUS_ERROR     = 0x01
STATUS_BUSY      = 0x02
STATUS_CRC_ERROR = 0x03
STATUS_TIMEOUT   = 0x04

STATUS_NAMES = {
    STATUS_OK:        "OK",
    STATUS_ERROR:     "ERROR",
    STATUS_BUSY:      "BUSY",
    STATUS_CRC_ERROR: "CRC_ERROR",
    STATUS_TIMEOUT:   "TIMEOUT",
}

CHUNK_WORDS  = 256
FLASH_WORDS  = 0x100000
FLASH_BYTES  = 0x200000
SLOT_WORDS   = 0x080000
SLOT_BYTES   = 0x100000

EXPECTED_MFR = 0x0020
EXPECTED_DEV = 0x22D2

# M29F160FT sector map: (start_word_addr, size_words)
# M29F160FT sector maps in AVR address space (0x000000-0x07FFFF).
# A19 is the hardware switch — the firmware always uses this range.
# Slot 0 (A19=0): 16 uniform 64KB sectors (lower half of flash)
SECTOR_MAP_SLOT0 = [(i * 0x8000, 0x8000) for i in range(16)]
# Slot 1 (A19=1): 15 uniform 64KB + top-boot block (upper half of flash)
SECTOR_MAP_SLOT1 = (
    [(i * 0x8000, 0x8000) for i in range(15)] +
    [(0x78000, 0x4000),
     (0x7C000, 0x1000),
     (0x7D000, 0x1000),
     (0x7E000, 0x2000)]
)
SECTOR_MAP = SECTOR_MAP_SLOT0  # default

# ---------------------------------------------------------------------------
# CRC-16/CCITT-FALSE
# ---------------------------------------------------------------------------
def crc16(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte << 8
        for _ in range(8):
            crc = ((crc << 1) ^ 0x1021) if (crc & 0x8000) else (crc << 1)
            crc &= 0xFFFF
    return crc

# ---------------------------------------------------------------------------
# Byte-swap: swap bytes within each 16-bit word
# ---------------------------------------------------------------------------
def byteswap_words(data: bytes) -> bytes:
    if len(data) % 2:
        raise ValueError("Data length must be even for 16-bit byte swap")
    arr = bytearray(len(data))
    for i in range(0, len(data), 2):
        arr[i]   = data[i + 1]
        arr[i+1] = data[i]
    return bytes(arr)

# ---------------------------------------------------------------------------
# Progress bar
# ---------------------------------------------------------------------------
def _progress(done: int, total: int) -> None:
    pct = done * 100 // total
    bar = '#' * (pct // 2) + '-' * (50 - pct // 2)
    print(f"\r  [{bar}] {pct:3d}%  {done:,}/{total:,} bytes",
          end='', flush=True)

# ---------------------------------------------------------------------------
# MeggyCom — USB transport layer using pyusb
# ---------------------------------------------------------------------------
class MeggyCom:
    def __init__(self):
        dev = usb.core.find(idVendor=USB_VID, idProduct=USB_PID)
        if dev is None:
            raise RuntimeError(
                "Meggy device not found.\n"
                "  - Is the firmware running (not in DFU mode)?\n"
                "  - Is the USB cable connected?\n"
                "  - Windows: has the WinUSB driver been installed via Zadig?"
            )

        # Detach kernel driver on Linux if needed
        if sys.platform != "win32":
            for iface in [0, 1]:
                try:
                    if dev.is_kernel_driver_active(iface):
                        dev.detach_kernel_driver(iface)
                except Exception:
                    pass

        dev.set_configuration()
        self.dev = dev

        # Internal receive buffer — holds leftover bytes from USB packets
        # that were larger than what _usb_read() requested at that moment.
        self._rxbuf = bytearray()

        # Flush any stale data sitting in the IN endpoint buffer
        try:
            while True:
                self.dev.read(EP_TX, EP_SIZE, timeout=50)
        except usb.core.USBTimeoutError:
            pass
        except usb.core.USBError:
            pass

    def close(self):
        usb.util.dispose_resources(self.dev)

    # -----------------------------------------------------------------------
    # Raw USB write — splits into EP_SIZE chunks automatically
    # -----------------------------------------------------------------------
    def _usb_write(self, data: bytes) -> None:
        offset = 0
        while offset < len(data):
            chunk = data[offset: offset + EP_SIZE]
            self.dev.write(EP_RX, chunk, timeout=2000)
            offset += len(chunk)

    # -----------------------------------------------------------------------
    # Raw USB read — returns exactly 'count' bytes.
    # Buffers any extra bytes received in a USB packet so they are
    # available for the next call instead of being discarded.
    # This is essential because pyusb returns full USB packets at once
    # even when we only ask for part of the data.
    # -----------------------------------------------------------------------
    def _usb_read(self, count: int, timeout_ms: int = 5000) -> bytes:
        deadline = time.time() + timeout_ms / 1000.0

        while len(self._rxbuf) < count:
            if time.time() > deadline:
                raise TimeoutError(
                    f"USB read timeout: got {len(self._rxbuf)}/{count} bytes"
                )
            remaining_ms = int((deadline - time.time()) * 1000)
            if remaining_ms <= 0:
                raise TimeoutError(
                    f"USB read timeout: got {len(self._rxbuf)}/{count} bytes"
                )
            try:
                # Read one USB packet — may be up to EP_SIZE bytes.
                # All bytes go into the buffer; we return exactly 'count'.
                packet = self.dev.read(EP_TX, EP_SIZE,
                                       timeout=min(remaining_ms, 500))
                self._rxbuf.extend(bytes(packet))
            except usb.core.USBTimeoutError:
                if time.time() >= deadline:
                    raise TimeoutError(
                        f"USB read timeout: got {len(self._rxbuf)}/{count} bytes"
                    )

        # Return exactly count bytes, leave the rest in the buffer
        result       = bytes(self._rxbuf[:count])
        self._rxbuf  = self._rxbuf[count:]
        return result

    # -----------------------------------------------------------------------
    # Packet builder — same format as before
    # -----------------------------------------------------------------------
    def _build_packet(self, cmd: int, addr: int,
                      length_words: int, payload: bytes = b'') -> bytes:
        header = bytes([
            MAGIC[0], MAGIC[1],
            cmd,
            (addr         >> 16) & 0xFF,
            (addr         >>  8) & 0xFF,
             addr                & 0xFF,
            (length_words >>  8) & 0xFF,
             length_words        & 0xFF,
        ])
        body     = header + payload
        checksum = crc16(body)
        return body + bytes([checksum >> 8, checksum & 0xFF])

    # -----------------------------------------------------------------------
    # Receive and validate one response packet
    # -----------------------------------------------------------------------
    def _recv_response(self) -> tuple:
        header = self._usb_read(5)
        if header[0:2] != MAGIC:
            raise ValueError(f"Bad magic in response: {header[0:2].hex()}")

        status       = header[2]
        length_words = (header[3] << 8) | header[4]
        data_bytes   = length_words * 2

        rest = self._usb_read(data_bytes + 2) if (data_bytes + 2) > 0 \
               else self._usb_read(2)

        payload  = rest[:data_bytes]
        crc_got  = (rest[-2] << 8) | rest[-1]
        crc_calc = crc16(bytes(header) + bytes(rest[:-2]))

        if crc_got != crc_calc:
            raise ValueError(
                f"CRC mismatch: got 0x{crc_got:04X} expected 0x{crc_calc:04X}"
            )
        return status, bytes(payload)

    # -----------------------------------------------------------------------
    # Send + receive with retries
    # -----------------------------------------------------------------------
    def _transact(self, cmd: int, addr: int = 0,
                  length_words: int = 0, payload: bytes = b'',
                  retries: int = 3) -> tuple:
        last_exc = None
        for attempt in range(retries):
            try:
                pkt = self._build_packet(cmd, addr, length_words, payload)
                self._usb_write(pkt)
                return self._recv_response()
            except (TimeoutError, ValueError, usb.core.USBError) as e:
                last_exc = e
                # Flush stale data and clear internal buffer before retry
                self._rxbuf.clear()
                try:
                    while True:
                        self.dev.read(EP_TX, EP_SIZE, timeout=50)
                except Exception:
                    pass
                time.sleep(0.15 * (attempt + 1))
        raise last_exc

    # -----------------------------------------------------------------------
    # Flash operations
    # -----------------------------------------------------------------------

    def identify(self) -> tuple:
        status, data = self._transact(CMD_IDENTIFY)
        if status != STATUS_OK or len(data) < 4:
            raise RuntimeError(
                f"IDENTIFY failed: {STATUS_NAMES.get(status, hex(status))}"
            )
        # AVR stores uint16_t little-endian (low byte first).
        # Parse accordingly — big-endian interpretation gives wrong values.
        mfr = data[0] | (data[1] << 8)
        dev = data[2] | (data[3] << 8)
        return mfr, dev

    def flash_reset(self, verbose: bool = False) -> None:
        status, data = self._transact(CMD_FLASH_RESET)
        if status != STATUS_OK:
            raise RuntimeError("FLASH_RESET failed")
        if verbose and len(data) >= 4:
            w0 = data[0] | (data[1] << 8)
            w1 = data[2] | (data[3] << 8)
            print(f"  Flash reset: word[0]=0x{w0:04X} word[1]=0x{w1:04X}")
            # Autoselect responses have 0x21 in the high byte (ST/Micron)
            # or 0x01 in the high byte — any 0x21xx value is autoselect
            if (w0 >> 8) == 0x21 or (w0 >> 8) == 0x01 or w0 == w1:
                print(f"  WARNING: Flash still in autoselect/error state! (0x{w0:04X})")
                print(f"  Try power cycling the Meggy board (unplug USB, wait, replug).")
            else:
                print(f"  Flash is in read mode.")

    def get_status(self) -> int:
        status, _ = self._transact(CMD_GET_STATUS)
        return status

    def wait_ready(self, timeout_s: float = 220.0,
                   poll_interval: float = 0.5,
                   show_dots: bool = True) -> None:
        deadline = time.time() + timeout_s
        dots = 0
        while time.time() < deadline:
            s = self.get_status()
            if s == STATUS_OK:
                if show_dots and dots:
                    print()
                return
            if s != STATUS_BUSY:
                raise RuntimeError(
                    f"Unexpected status: {STATUS_NAMES.get(s, hex(s))}"
                )
            if show_dots:
                print(".", end="", flush=True)
                dots += 1
            time.sleep(poll_interval)
        raise TimeoutError("Flash did not become ready within timeout")

    def chip_erase(self) -> None:
        status, _ = self._transact(CMD_CHIP_ERASE)
        if status not in (STATUS_OK, STATUS_BUSY):
            raise RuntimeError(
                f"CHIP_ERASE failed: {STATUS_NAMES.get(status, hex(status))}"
            )

    def sector_erase(self, word_addr: int) -> None:
        status, _ = self._transact(CMD_SECTOR_ERASE, addr=word_addr)
        if status not in (STATUS_OK, STATUS_BUSY):
            raise RuntimeError(
                f"SECTOR_ERASE failed at 0x{word_addr:06X}: "
                f"{STATUS_NAMES.get(status, hex(status))}"
            )

    def read(self, word_addr: int, word_count: int,
             show_progress: bool = True) -> bytes:
        # Always reset flash to read mode before any read operation.
        # This ensures we are not reading autoselect/status data if
        # the flash was left in a non-read state by a previous command.
        self.flash_reset()

        result       = bytearray()
        words_left   = word_count
        current_addr = word_addr
        total_bytes  = word_count * 2

        if show_progress:
            print(f"Reading {total_bytes:,} bytes...")

        while words_left > 0:
            chunk = min(words_left, CHUNK_WORDS)
            status, data = self._transact(
                CMD_READ,
                addr=current_addr,
                length_words=chunk,
                retries=5,
            )
            if status != STATUS_OK:
                raise RuntimeError(
                    f"READ failed at 0x{current_addr:06X}: "
                    f"{STATUS_NAMES.get(status, hex(status))}"
                )
            if len(data) != chunk * 2:
                raise RuntimeError(
                    f"READ: got {len(data)} bytes, expected {chunk * 2}"
                )
            result.extend(data)
            current_addr += chunk
            words_left   -= chunk
            if show_progress:
                _progress(len(result), total_bytes)

        if show_progress:
            print()
        return bytes(result)

    def program(self, word_addr: int, data: bytes,
                show_progress: bool = True) -> None:
        if len(data) % 2:
            raise ValueError("Data length must be even")

        total_bytes  = len(data)
        words_left   = total_bytes // 2
        current_addr = word_addr
        offset       = 0

        if show_progress:
            print(f"Programming {total_bytes:,} bytes...")

        while words_left > 0:
            chunk_w = min(words_left, CHUNK_WORDS)
            chunk_b = chunk_w * 2
            payload = data[offset: offset + chunk_b]
            status, _ = self._transact(
                CMD_PROGRAM,
                addr=current_addr,
                length_words=chunk_w,
                payload=payload,
            )
            if status != STATUS_OK:
                raise RuntimeError(
                    f"PROGRAM failed at 0x{current_addr:06X}: "
                    f"{STATUS_NAMES.get(status, hex(status))}"
                )
            current_addr += chunk_w
            words_left   -= chunk_w
            offset       += chunk_b
            if show_progress:
                _progress(offset, total_bytes)

        if show_progress:
            print()

    def verify(self, word_addr: int, expected: bytes,
               show_progress: bool = True) -> bool:
        if show_progress:
            print("Verifying...")
        actual = self.read(word_addr, len(expected) // 2,
                           show_progress=show_progress)
        if actual == expected:
            if show_progress:
                print("Verify OK.")
            return True
        for i, (a, b) in enumerate(zip(actual, expected)):
            if a != b:
                wa = word_addr + i // 2
                print(f"\nVERIFY FAILED byte 0x{i:06X} "
                      f"(word 0x{wa:06X}): "
                      f"read 0x{a:02X} expected 0x{b:02X}")
                break
        return False


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
def erase_chip(dev: MeggyCom) -> None:
    print("Erasing entire chip (~30 seconds)...")
    print("  (A19 switch position does not matter for chip erase)")
    dev.chip_erase()
    dev.wait_ready(timeout_s=220)
    dev.flash_reset()
    print("Chip erase complete.")

def erase_slot(dev: MeggyCom, slot: int) -> None:
    # A19 is hardware — use AVR addresses 0x000000-0x07FFFF for both slots.
    secs = SECTOR_MAP_SLOT1 if slot == 1 else SECTOR_MAP_SLOT0
    print(f"Erasing {len(secs)} sectors for image slot {slot}...")
    print(f"  (ensure A19 switch is in slot {slot} position)")
    for i, (s_addr, s_size) in enumerate(secs):
        print(f"  Sector {i+1}/{len(secs)} addr=0x{s_addr:06X}...", end='', flush=True)
        dev.sector_erase(s_addr)
        dev.wait_ready(timeout_s=30, show_dots=False)
        dev.flash_reset()
        check = dev.read(s_addr, 1, show_progress=False)
        word = check[0] | (check[1] << 8)
        if word == 0xFFFF:
            print(" blank OK")
        else:
            print(f" WARNING: 0x{word:04X} != 0xFFFF - NOT ERASED!")
    print(f"  All {len(secs)} sectors erased.")

def load_image(path: str, swap: bool, slot) -> tuple:
    with open(path, "rb") as f:
        data = f.read()
    print(f"Loaded {len(data):,} bytes from {path}")
    if swap:
        data = byteswap_words(data)
        print("16-bit byte-swap applied.")
    max_bytes = SLOT_BYTES

    # A19 is controlled by the hardware switch — the firmware always
    # addresses 0x000000-0x07FFFF regardless of which slot is selected.
    # word_addr is therefore always 0.
    word_addr = 0

    if len(data) > max_bytes:
        print(f"ERROR: Image ({len(data):,} B) exceeds 1 MB slot ({max_bytes:,} B).")
        sys.exit(1)
    if len(data) < max_bytes:
        pad = max_bytes - len(data)
        print(f"Padding with 0xFF (+{pad:,} bytes).")
        data += b'\xFF' * pad
    return data, word_addr

# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------
def parse_args():
    p = argparse.ArgumentParser(
        description="Meggy ROM board flash programmer (USB/WinUSB)",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Setup:
  Windows: install WinUSB driver for "Meggy Flash Programmer" via Zadig
  Linux:   pip install pyusb  (may also need a udev rule, see below)

Examples:
  python meggy_flash.py --identify
  python meggy_flash.py --read dump.bin
  python meggy_flash.py --erase
  python meggy_flash.py --write ks314_1mb.rom --swap --image 0
  python meggy_flash.py --write diagrom.bin   --swap --image 1
  python meggy_flash.py --verify ks314_1mb.rom --swap --image 0

Linux udev rule (create /etc/udev/rules.d/99-meggy.rules):
  SUBSYSTEM=="usb", ATTR{idVendor}=="03eb", ATTR{idProduct}=="2044", MODE="0666"
        """,
    )
    p.add_argument("--identify",  action="store_true",
                   help="Read flash manufacturer/device ID")
    p.add_argument("--read",   metavar="FILE",
                   help="Dump full 2 MB flash to FILE")
    p.add_argument("--read-slot", metavar="FILE",
                   help="Dump the currently selected 1 MB slot to FILE "
                        "(set A19 switch first, then run this)")
    p.add_argument("--write",  metavar="FILE",
                   help="Erase and program flash from FILE")
    p.add_argument("--verify", metavar="FILE",
                   help="Verify flash against FILE (no programming)")
    p.add_argument("--erase",  action="store_true",
                   help="Full chip erase (or slot erase if --image given)")
    p.add_argument("--swap",   action="store_true",
                   help="16-bit byte-swap FILE before programming/verifying")
    p.add_argument("--image",  type=int, choices=[0, 1], metavar="N",
                   help="Image slot: 0=A19 low, 1=A19 high")
    p.add_argument("--reset",  action="store_true",
                   help="Send AMD reset command to flash")
    p.add_argument("--avr-reset", action="store_true",
                   help="Trigger AVR watchdog reset (reboots firmware)")
    p.add_argument("--selftest", action="store_true",
                   help="Write/read/verify a known pattern to sector 0 (diagnostic)")
    p.add_argument("--loopback", action="store_true",
                   help="Send a raw GET_STATUS command and print raw bytes (diagnostic)")
    return p.parse_args()


def main():
    args = parse_args()

    if not any([args.identify, args.read, args.write,
                args.verify, args.erase, args.reset, args.loopback,
                args.selftest, args.read_slot, args.avr_reset]):
        print("No action specified.  Use --help for usage.")
        sys.exit(1)

    try:
        dev = MeggyCom()
    except RuntimeError as e:
        print(f"ERROR: {e}")
        sys.exit(1)

    try:
        if args.selftest:
            print("Self-test: erase sector 0, write incrementing pattern, verify...")
            print("  WARNING: This erases sector 0 (first 64KB) of the flash.")

            # Build 512-byte test pattern (256 words of incrementing values)
            test_words = 256
            test_data  = bytearray()
            for i in range(test_words):
                test_data += bytes([i & 0xFF, (i >> 8) & 0xFF])  # little-endian words
            test_data = bytes(test_data)

            # Erase sector 0
            print("  Erasing sector 0...")
            dev.sector_erase(0x0000)
            dev.wait_ready(timeout_s=30, show_dots=True)

            # Verify sector is blank (all 0xFF)
            print("  Checking blank...")
            blank = dev.read(0, test_words, show_progress=False)
            if blank != b'\xFF' * (test_words * 2):
                print(f"  WARNING: Sector not fully blank after erase.")
                print(f"  First 16 bytes: {blank[:16].hex()}")
            else:
                print("  Sector blank OK.")

            # Write test pattern
            print("  Writing incrementing pattern (256 words)...")
            dev.program(0x0000, test_data, show_progress=False)

            # Read back
            print("  Reading back...")
            readback = dev.read(0, test_words, show_progress=False)

            # Compare
            if readback == test_data:
                print("  SELF-TEST PASSED — read/write is consistent.")
                print("  Address and data buses are working correctly.")
            else:
                print("  SELF-TEST FAILED — mismatch detected.")
                # Find first difference
                for i in range(0, len(test_data), 2):
                    exp_w = test_data[i] | (test_data[i+1] << 8)
                    got_w = readback[i] | (readback[i+1] << 8)
                    if exp_w != got_w:
                        print(f"  First mismatch at word 0x{i//2:04X}: "
                              f"wrote 0x{exp_w:04X}, read 0x{got_w:04X}")
                        print(f"  Written bits:  {exp_w:016b}")
                        print(f"  Readback bits: {got_w:016b}")
                        print(f"  XOR (wrong):   {(exp_w^got_w):016b}")
                        break
                # Show first 32 bytes of each
                print(f"  Expected:  {test_data[:32].hex()}")
                print(f"  Got:       {readback[:32].hex()}")

        if args.loopback:
            print("Sending raw GET_STATUS command (10-byte packet)...")
            from meggy_flash import crc16, MAGIC, CMD_GET_STATUS
            pkt = bytes([MAGIC[0], MAGIC[1], CMD_GET_STATUS,
                         0, 0, 0, 0, 0])
            chk = crc16(pkt)
            pkt += bytes([chk >> 8, chk & 0xFF])
            print(f"  Sending:  {pkt.hex()}")
            dev._usb_write(pkt)
            print("  Packet sent. Waiting for response...")
            try:
                raw = dev.dev.read(EP_TX, 64, timeout=3000)
                print(f"  Received: {bytes(raw).hex()}")
            except Exception as e:
                print(f"  Read failed: {e}")

        if args.reset:
            dev.flash_reset()
            print("Flash reset issued.")

        if args.avr_reset:
            print("Sending AVR reset command...")
            status, _ = dev._transact(CMD_AVR_RESET)
            if status == STATUS_OK:
                print("AVR is resetting — reconnect USB in a moment.")
            else:
                print(f"AVR reset failed: {STATUS_NAMES.get(status, hex(status))}")

        if args.identify:
            print("Reading flash ID...")
            mfr, did = dev.identify()
            print(f"  Manufacturer : 0x{mfr:04X}  "
                  f"({'ST/Micron' if mfr == EXPECTED_MFR else 'UNKNOWN'})")
            print(f"  Device       : 0x{did:04X}  "
                  f"({'M29F160FT (top-boot)' if did == EXPECTED_DEV else 'UNKNOWN'})")
            if did == EXPECTED_DEV and mfr != EXPECTED_MFR:
                print(f"  NOTE: Device ID is correct (M29F160FT confirmed).")
                print(f"  Manufacturer ID mismatch — likely a cold solder joint")
                print(f"  on flash D5 (PC5). Programming will still work.")
            elif mfr != EXPECTED_MFR or did != EXPECTED_DEV:
                print("  WARNING: IDs do not match M29F160FT55N3E2.")

        if args.erase:
            if args.image is None:
                erase_chip(dev)
            else:
                erase_slot(dev, args.image)

        if args.write:
            if not os.path.isfile(args.write):
                print(f"ERROR: File not found: {args.write}")
                sys.exit(1)
            data, word_addr = load_image(args.write, args.swap, args.image)
            if not args.erase:
                if args.image is None:
                    erase_chip(dev)
                else:
                    erase_slot(dev, args.image)
            dev.program(word_addr, data)
            # Give flash time to fully exit post-program state
            # before starting verify reads
            print("  Waiting for flash to settle...")
            import time as _time; _time.sleep(2)
            dev.flash_reset(verbose=False)
            if not dev.verify(word_addr, data):
                print("ERROR: Verification failed!")
                sys.exit(1)

        if args.read:
            print(f"Reading full 2 MB flash to {args.read}...")
            raw = dev.read(0, FLASH_WORDS)
            with open(args.read, "wb") as f:
                f.write(raw)
            print(f"Saved {len(raw):,} bytes to {args.read}")

        if args.read_slot:
            print(f"Reading current 1 MB slot to {args.read_slot}...")
            print("  (reads AVR word addresses 0x000000-0x07FFFF;")
            print("   set A19 switch to desired slot before reading)")
            raw = dev.read(0, SLOT_WORDS)
            with open(args.read_slot, "wb") as f:
                f.write(raw)
            print(f"Saved {len(raw):,} bytes to {args.read_slot}")
            print()
            print("To verify against your original ROM file:")
            print("  python meggy_flash.py --verify <romfile> --swap --image 0")
            print("  (--image 0 targets word addr 0 regardless of switch position)")

        if args.verify and not args.write:
            if not os.path.isfile(args.verify):
                print(f"ERROR: File not found: {args.verify}")
                sys.exit(1)
            data, word_addr = load_image(args.verify, args.swap, args.image)
            ok = dev.verify(word_addr, data)
            sys.exit(0 if ok else 1)

    except KeyboardInterrupt:
        print("\nAborted.")
        sys.exit(1)
    except (RuntimeError, TimeoutError, ValueError) as e:
        print(f"\nERROR: {e}")
        sys.exit(1)
    finally:
        dev.close()


if __name__ == "__main__":
    main()

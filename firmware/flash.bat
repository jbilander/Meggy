@echo off
:: ============================================================
:: Meggy firmware flash script
:: Enter DFU mode first: hold HWB, power up device, release HWB
:: ============================================================

set AVR_PATH=C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
set AVRDUDE="%AVR_PATH%\avrdude.exe"
set CONF="%AVR_PATH%\..\etc\avrdude.conf"
set TARGET=meggy_firmware.hex

if not exist %TARGET% (
    echo ERROR: %TARGET% not found — run build.bat first.
    exit /b 1
)

echo Flashing %TARGET% ...
:: avrdude returns non-zero due to fuse read failures which are harmless.
:: Check for success by looking for the verified line instead.
%AVRDUDE% -C%CONF% -c flip1 -p usb1286 -U flash:w:%TARGET%:i 2>&1 | findstr /C:"bytes of flash verified" >nul

if errorlevel 1 (
    echo.
    echo FLASH FAILED.
    echo Make sure the device is in DFU mode:
    echo   Hold HWB, power up device, release HWB
    exit /b 1
)

echo.
echo Flash successful.
echo.
echo Triggering AVR reset...
python ..\meggy_flash.py --avr-reset 2>nul || echo (Skipped - install pyusb and WinUSB driver to enable auto-reset)
echo Done.

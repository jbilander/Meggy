@echo off
:: ============================================================
:: Meggy firmware build script
:: Works like blink build.bat — no make or WSL needed.
::
:: Prerequisites:
::   1. Arduino IDE installed at default location, OR edit
::      AVR_PATH below to match your installation.
::   2. LUFA cloned into the firmware\LUFA folder:
::        cd firmware
::        git clone https://github.com/abcminiuser/lufa LUFA
::
:: Usage:
::   build.bat          — compile only
::   build.bat clean    — delete build output
:: ============================================================

set AVR_PATH=C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
set CC="%AVR_PATH%\avr-gcc.exe"
set OBJCOPY="%AVR_PATH%\avr-objcopy.exe"
set SIZE="%AVR_PATH%\avr-size.exe"

set MCU=at90usb1286
set F_CPU=16000000UL
set F_USB=16000000UL
set TARGET=meggy_firmware
set LUFA=LUFA\LUFA

:: ---- Clean ----
if "%1"=="clean" (
    echo Cleaning...
    del /q *.o *.elf *.hex 2>nul
    exit /b 0
)

:: ---- Check LUFA is present ----
if not exist "%LUFA%\Drivers\USB\USB.h" (
    echo ERROR: LUFA not found in the LUFA\ subfolder.
    echo.
    echo Clone it first:
    echo   cd firmware
    echo   git clone https://github.com/abcminiuser/lufa LUFA
    exit /b 1
)

:: ---- Compiler flags ----
set CFLAGS=-mmcu=%MCU% -DF_CPU=%F_CPU% -DF_USB=%F_USB%
set CFLAGS=%CFLAGS% -DARCH=ARCH_AVR8 -DBOARD=BOARD_NONE
set CFLAGS=%CFLAGS% -DUSE_LUFA_CONFIG_HEADER
set CFLAGS=%CFLAGS% -I. -I%LUFA%\..
set CFLAGS=%CFLAGS% -Os -std=gnu99 -Wall -Wextra -Wno-unused-parameter
set CFLAGS=%CFLAGS% -ffunction-sections -fdata-sections

:: ---- Source files — project ----
set SRC=main.c flash.c protocol.c Descriptors.c

:: ---- Source files — LUFA USB core (AVR8 device) ----
set LUFA_USB=%LUFA%\Drivers\USB

set SRC=%SRC% %LUFA_USB%\Core\AVR8\Device_AVR8.c
set SRC=%SRC% %LUFA_USB%\Core\AVR8\EndpointStream_AVR8.c
set SRC=%SRC% %LUFA_USB%\Core\AVR8\Endpoint_AVR8.c
set SRC=%SRC% %LUFA_USB%\Core\AVR8\USBController_AVR8.c
set SRC=%SRC% %LUFA_USB%\Core\AVR8\USBInterrupt_AVR8.c
set SRC=%SRC% %LUFA_USB%\Core\ConfigDescriptors.c
set SRC=%SRC% %LUFA_USB%\Core\DeviceStandardReq.c
set SRC=%SRC% %LUFA_USB%\Core\Events.c
set SRC=%SRC% %LUFA_USB%\Core\USBTask.c

:: CDC class driver not used — plain bulk endpoints only

:: ---- Compile all sources to one .elf in a single pass ----
echo Building %TARGET%.hex ...
%CC% %CFLAGS% -Wl,--gc-sections -o %TARGET%.elf %SRC%

if errorlevel 1 (
    echo.
    echo BUILD FAILED.
    exit /b 1
)

:: ---- Convert to Intel HEX ----
%OBJCOPY% -O ihex -R .eeprom %TARGET%.elf %TARGET%.hex

:: ---- Show size ----
%SIZE% %TARGET%.elf

echo.
echo Build successful: %TARGET%.hex
echo.
echo To flash:
echo   1. Hold SW1 (HWB)
echo   2. Run: python meggy_flash.py --avr-reset
echo   3. Release SW1 when avrdude connects
echo   4. Run: flash.bat

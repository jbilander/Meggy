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
%AVRDUDE% -C%CONF% -c flip1 -p usb1286 -U flash:w:%TARGET%:i

if errorlevel 1 (
    echo.
    echo FLASH FAILED.
    echo Make sure the device is in DFU mode:
    echo   Hold HWB, power up device, release HWB
    exit /b 1
)

echo.
echo Flash successful. Device is running new firmware.

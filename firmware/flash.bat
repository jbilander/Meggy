@echo off
:: ============================================================
:: Meggy firmware flash script
:: ============================================================

set AVR_PATH=C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
set AVRDUDE="%AVR_PATH%\avrdude.exe"
set CONF="%AVR_PATH%\..\etc\avrdude.conf"
set TARGET=meggy_firmware.hex
set LOGFILE=%TEMP%\avrdude_out.txt

if not exist %TARGET% (
    echo ERROR: %TARGET% not found - run build.bat first.
    exit /b 1
)

echo Flashing %TARGET% ...
%AVRDUDE% -C%CONF% -c flip1 -p usb1286 -U flash:w:%TARGET%:i > "%LOGFILE%" 2>&1
findstr /C:"bytes of flash verified" "%LOGFILE%" >nul

if errorlevel 1 (
    echo.
    type "%LOGFILE%"
    echo.
    echo FLASH FAILED.
    echo Make sure the device is in DFU mode:
    echo   1. Hold SW1 ^(HWB^)
    echo   2. Run: python meggy_flash.py --avr-reset
    echo   3. Release SW1 when avrdude connects
    del "%LOGFILE%" >nul 2>&1
    exit /b 1
)

del "%LOGFILE%" >nul 2>&1
echo.
echo Flash successful.
echo To return to normal mode, power cycle the board:
echo   - Standalone: unplug and replug USB
echo   - Installed in Amiga: disconnect USB and power off Amiga, then reconnect
echo.
echo To enter DFU mode for next update:
echo   1. Hold SW1 ^(HWB^)
echo   2. Run: python meggy_flash.py --avr-reset
echo   3. Release SW1 when avrdude connects
echo   4. Run: flash.bat

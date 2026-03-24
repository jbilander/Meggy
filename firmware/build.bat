"C:\\Program Files (x86)\\Arduino\\hardware\\tools\\avr/bin/avr-gcc" -DF_CPU=16000000UL -Os -mmcu=at90usb1286 main.c -o main.elf
"C:\\Program Files (x86)\\Arduino\\hardware\\tools\\avr/bin/avr-objcopy" -O ihex main.elf main.hex

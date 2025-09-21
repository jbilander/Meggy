EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L power:VCC #PWR0101
U 1 1 66BFF4F3
P 1050 1100
F 0 "#PWR0101" H 1065 1273 50  0001 C CNN
F 1 "VCC" H 1065 1273 50  0001 C CNN
F 2 "" H 1050 1100 50  0001 C CNN
F 3 "" H 1050 1100 50  0001 C CNN
	1    1050 1100
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 66BFFF76
P 1050 1200
F 0 "#FLG0101" H 1050 1275 50  0001 C CNN
F 1 "PWR_FLAG" H 1050 1373 50  0001 C CNN
F 2 "" H 1050 1200 50  0001 C CNN
F 3 "~" H 1050 1200 50  0001 C CNN
	1    1050 1200
	-1   0    0    1   
$EndComp
Text GLabel 1000 1150 0    50   Input ~ 0
VCC
Wire Wire Line
	1050 1100 1050 1150
Wire Wire Line
	1000 1150 1050 1150
Connection ~ 1050 1150
Wire Wire Line
	1050 1150 1050 1200
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 66C0324F
P 1200 1100
F 0 "#FLG0102" H 1200 1175 50  0001 C CNN
F 1 "PWR_FLAG" H 1200 1273 50  0001 C CNN
F 2 "" H 1200 1100 50  0001 C CNN
F 3 "~" H 1200 1100 50  0001 C CNN
	1    1200 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 66C03B19
P 1200 1200
F 0 "#PWR0102" H 1200 950 50  0001 C CNN
F 1 "GND" H 1205 1027 50  0001 C CNN
F 2 "" H 1200 1200 50  0001 C CNN
F 3 "" H 1200 1200 50  0001 C CNN
	1    1200 1200
	1    0    0    -1  
$EndComp
Text GLabel 1250 1150 2    50   Input ~ 0
GND
Wire Wire Line
	1200 1100 1200 1150
Wire Wire Line
	1250 1150 1200 1150
Connection ~ 1200 1150
Wire Wire Line
	1200 1150 1200 1200
Text GLabel 1250 3950 3    50   Input ~ 0
GND
$Comp
L Connector:USB_B_Micro J1
U 1 1 68CB996A
P 1250 3550
F 0 "J1" H 1307 4017 50  0000 C CNN
F 1 "USB_B_Micro" H 1307 3926 50  0000 C CNN
F 2 "Meggy:101181940001LF" H 1400 3500 50  0001 C CNN
F 3 "~" H 1400 3500 50  0001 C CNN
	1    1250 3550
	1    0    0    -1  
$EndComp
$Comp
L Meggy:ROM-42pin U1
U 1 1 68C9F45B
P 7400 3400
F 0 "U1" H 7400 4617 50  0000 C CNN
F 1 "ROM-socket-42pin" H 7400 4526 50  0000 C CNN
F 2 "Package_DIP:DIP-42_W15.24mm" H 6850 2150 50  0001 L CNN
F 3 "" H 7400 3150 50  0001 C CNN
	1    7400 3400
	1    0    0    -1  
$EndComp
Text Label 2200 3350 0    50   ~ 0
VBUS
$Comp
L Device:R_Small R1
U 1 1 68CDBA06
P 1700 3650
F 0 "R1" V 1800 3650 50  0000 C CNN
F 1 "22" V 1700 3650 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" H 1700 3650 50  0001 C CNN
F 3 "~" H 1700 3650 50  0001 C CNN
	1    1700 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	1550 3550 1600 3550
Wire Wire Line
	1550 3650 1600 3650
$Comp
L Device:R_Small R4
U 1 1 68CDBE7C
P 1950 3350
F 0 "R4" V 1850 3350 50  0000 C CNN
F 1 "100" V 1950 3350 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" H 1950 3350 50  0001 C CNN
F 3 "~" H 1950 3350 50  0001 C CNN
	1    1950 3350
	0    1    1    0   
$EndComp
NoConn ~ 1550 3750
Wire Wire Line
	1150 3950 1250 3950
Text GLabel 5850 3500 2    50   Input ~ 0
BOARD_5V
Text GLabel 7900 4450 2    50   Output ~ 0
VCC
Text GLabel 6900 3550 0    50   Input ~ 0
GND
Text GLabel 7900 3550 2    50   Input ~ 0
GND
Text GLabel 7900 3450 2    50   Input ~ 0
NC2
Text GLabel 3050 6250 3    50   Input ~ 0
GND
Text GLabel 7900 2450 2    50   Input ~ 0
NC
$Comp
L Meggy:M29F160F U3
U 1 1 68CB26C5
P 5350 3500
F 0 "U3" H 5350 4825 50  0000 C CNN
F 1 "M29F160F" H 5350 4734 50  0000 C CNN
F 2 "Package_SO:TSOP-I-48_18.4x12mm_P0.5mm" H 5350 4050 50  0001 C CNN
F 3 "" H 5350 4050 50  0001 C CNN
	1    5350 3500
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_AVR:AT90USB1286-MU U2
U 1 1 68CB469F
P 3150 4250
F 0 "U2" H 3150 4400 50  0000 C CNN
F 1 "AT90USB1286-MU" H 3150 4250 50  0000 C CNN
F 2 "Meggy:QFN50P900X900X100-65N-D" H 3150 4250 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/doc7593.pdf" H 3150 4250 50  0001 C CNN
	1    3150 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 6250 3150 6250
Wire Wire Line
	2550 3350 2050 3350
Wire Wire Line
	1550 3350 1700 3350
Text Label 2200 3550 0    50   ~ 0
D+
Text Label 2200 3650 0    50   ~ 0
D-
$Comp
L Device:R_Small R2
U 1 1 68CCF898
P 1700 3550
F 0 "R2" V 1800 3550 50  0000 C CNN
F 1 "22" V 1700 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad0.98x0.95mm_HandSolder" H 1700 3550 50  0001 C CNN
F 3 "~" H 1700 3550 50  0001 C CNN
	1    1700 3550
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1800 3650 2550 3650
Wire Wire Line
	1800 3550 2550 3550
Text GLabel 5850 4700 2    50   Input ~ 0
A0
Text GLabel 6900 3350 0    50   Input ~ 0
A0
Text GLabel 4850 4600 0    50   Input ~ 0
A2
Text GLabel 6900 3150 0    50   Input ~ 0
A2
Text GLabel 4850 4500 0    50   Input ~ 0
A3
Text GLabel 6900 3050 0    50   Input ~ 0
A3
Text GLabel 4850 4400 0    50   Input ~ 0
A4
Text GLabel 6900 2950 0    50   Input ~ 0
A4
Text GLabel 4850 4300 0    50   Input ~ 0
A5
Text GLabel 6900 2850 0    50   Input ~ 0
A5
Text GLabel 4850 4200 0    50   Input ~ 0
A6
Text GLabel 6900 2750 0    50   Input ~ 0
A6
Text GLabel 4850 4100 0    50   Input ~ 0
A7
Text GLabel 6900 2650 0    50   Input ~ 0
A7
Text GLabel 4850 3100 0    50   Input ~ 0
A8
Text GLabel 7900 2550 2    50   Input ~ 0
A8
Text GLabel 5850 4500 2    50   Input ~ 0
GND
Text GLabel 5850 2600 2    50   Input ~ 0
GND
Text GLabel 4850 3000 0    50   Input ~ 0
A9
Text GLabel 7900 2650 2    50   Input ~ 0
A9
Text GLabel 7900 2750 2    50   Input ~ 0
A10
Text GLabel 4850 2900 0    50   Input ~ 0
A10
Text GLabel 4850 2800 0    50   Input ~ 0
A11
Text GLabel 7900 2850 2    50   Input ~ 0
A11
Text GLabel 7900 2950 2    50   Input ~ 0
A12
Text GLabel 4850 2700 0    50   Input ~ 0
A12
Text GLabel 4850 2600 0    50   Input ~ 0
A13
Text GLabel 7900 3050 2    50   Input ~ 0
A13
Text GLabel 7900 3150 2    50   Input ~ 0
A14
Text GLabel 4850 2500 0    50   Input ~ 0
A14
Text GLabel 4850 2400 0    50   Input ~ 0
A15
Text GLabel 7900 3250 2    50   Input ~ 0
A15
Text GLabel 7900 3350 2    50   Input ~ 0
A16
Text GLabel 5850 2400 2    50   Input ~ 0
A16
Text GLabel 4850 4000 0    50   Input ~ 0
A17
Text GLabel 6900 2550 0    50   Input ~ 0
A17
Text GLabel 6900 2450 0    50   Input ~ 0
A18
Text GLabel 4850 3900 0    50   Input ~ 0
A18
Text GLabel 4500 3200 3    50   Input ~ 0
A19
Text GLabel 5850 2500 2    50   Input ~ 0
BYTE
$Comp
L Connector_Generic:Conn_01x03 J2
U 1 1 68D0214B
P 4250 3000
F 0 "J2" V 4214 2812 50  0000 R CNN
F 1 "Conn_01x03" V 4350 3200 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Vertical" H 4250 3000 50  0001 C CNN
F 3 "~" H 4250 3000 50  0001 C CNN
	1    4250 3000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4850 3200 4350 3200
Text GLabel 4250 3200 3    50   Input ~ 0
GND
NoConn ~ 4150 3200
Text GLabel 6900 3750 0    50   BiDi ~ 0
D0
Text GLabel 5850 4300 2    50   BiDi ~ 0
D0
Text GLabel 6900 3950 0    50   BiDi ~ 0
D1
Text GLabel 5850 4100 2    50   BiDi ~ 0
D1
Text GLabel 5850 3900 2    50   BiDi ~ 0
D2
Text GLabel 6900 4150 0    50   BiDi ~ 0
D2
Text GLabel 5850 3700 2    50   BiDi ~ 0
D3
Text GLabel 6900 4350 0    50   BiDi ~ 0
D3
Text GLabel 5850 3400 2    50   BiDi ~ 0
D4
Text GLabel 7900 4350 2    50   BiDi ~ 0
D4
Text GLabel 7900 4150 2    50   BiDi ~ 0
D5
Text GLabel 7900 3950 2    50   BiDi ~ 0
D6
Text GLabel 7900 3750 2    50   BiDi ~ 0
D7
Text GLabel 6900 3850 0    50   BiDi ~ 0
D8
Text GLabel 5850 3200 2    50   BiDi ~ 0
D5
Text GLabel 5850 3000 2    50   BiDi ~ 0
D6
Text GLabel 5850 2800 2    50   BiDi ~ 0
D7
Text GLabel 5850 4200 2    50   BiDi ~ 0
D8
Text GLabel 5850 4000 2    50   BiDi ~ 0
D9
Text GLabel 6900 4050 0    50   BiDi ~ 0
D9
Text GLabel 5850 3800 2    50   BiDi ~ 0
D10
Text GLabel 6900 4250 0    50   BiDi ~ 0
D10
Text GLabel 5850 3600 2    50   BiDi ~ 0
D11
Text GLabel 6900 4450 0    50   BiDi ~ 0
D11
Text GLabel 5850 3300 2    50   BiDi ~ 0
D12
Text GLabel 7900 4250 2    50   BiDi ~ 0
D12
Text GLabel 5850 3100 2    50   BiDi ~ 0
D13
Text GLabel 7900 4050 2    50   BiDi ~ 0
D13
Text GLabel 5850 2900 2    50   BiDi ~ 0
D14
Text GLabel 7900 3850 2    50   BiDi ~ 0
D14
Text GLabel 5850 2700 2    50   BiDi ~ 0
D15
Text GLabel 7900 3650 2    50   BiDi ~ 0
D15
Text GLabel 6900 3450 0    50   Input ~ 0
NC1
Wire Wire Line
	5850 4500 5850 4600
Text GLabel 3150 2250 1    50   Input ~ 0
BOARD_5V
Wire Wire Line
	3050 2250 3150 2250
Connection ~ 3150 2250
Wire Wire Line
	3150 2250 3250 2250
Text GLabel 3750 4350 2    50   BiDi ~ 0
D0
Text GLabel 3750 4450 2    50   BiDi ~ 0
D1
Text GLabel 3750 4550 2    50   BiDi ~ 0
D2
Text GLabel 3750 4650 2    50   BiDi ~ 0
D3
Text GLabel 3750 3450 2    50   BiDi ~ 0
D8
Text GLabel 3750 3550 2    50   BiDi ~ 0
D9
Text GLabel 3750 3650 2    50   BiDi ~ 0
D10
Text GLabel 3750 3750 2    50   BiDi ~ 0
D11
Text GLabel 3750 4750 2    50   BiDi ~ 0
D4
Text GLabel 3750 4850 2    50   BiDi ~ 0
D5
Text GLabel 3750 4950 2    50   BiDi ~ 0
D6
Text GLabel 3750 5050 2    50   BiDi ~ 0
D7
Text GLabel 3750 3850 2    50   BiDi ~ 0
D12
Text GLabel 3750 3950 2    50   BiDi ~ 0
D13
Text GLabel 3750 4050 2    50   BiDi ~ 0
D14
Text GLabel 3750 4150 2    50   BiDi ~ 0
D15
Text GLabel 3750 2550 2    50   Input ~ 0
A0
Text GLabel 3750 2750 2    50   Input ~ 0
A2
Text GLabel 3750 2850 2    50   Input ~ 0
A3
Text GLabel 3750 2950 2    50   Input ~ 0
A4
Text GLabel 3750 3050 2    50   Input ~ 0
A5
Text GLabel 3750 3150 2    50   Input ~ 0
A6
Text GLabel 3750 3250 2    50   Input ~ 0
A7
Text GLabel 3750 5250 2    50   Input ~ 0
A8
Text GLabel 3750 5350 2    50   Input ~ 0
A9
Text GLabel 3750 5450 2    50   Input ~ 0
A10
Text GLabel 3750 5550 2    50   Input ~ 0
A11
Text GLabel 3750 5650 2    50   Input ~ 0
A12
Text GLabel 3750 5750 2    50   Input ~ 0
A13
Text GLabel 3750 5850 2    50   Input ~ 0
A14
Text GLabel 3750 5950 2    50   Input ~ 0
A15
Text GLabel 2550 5750 0    50   Input ~ 0
A16
Text GLabel 2550 5950 0    50   Input ~ 0
A18
Text GLabel 2550 2950 0    50   Input ~ 0
XTAL2
Text GLabel 2550 2550 0    50   Input ~ 0
RESET
Text GLabel 2550 3150 0    50   Input ~ 0
AREF
Text GLabel 2550 4550 0    50   Input ~ 0
HWB
Text GLabel 2550 4650 0    50   Input ~ 0
PE3
Text GLabel 2550 4750 0    50   Input ~ 0
PE4
Text GLabel 2550 5250 0    50   Input ~ 0
PF0
Text GLabel 2550 5350 0    50   Input ~ 0
PF1
Text GLabel 2550 5450 0    50   Input ~ 0
PF2
Text GLabel 2550 4850 0    50   Input ~ 0
PE5
Text GLabel 2550 4950 0    50   Input ~ 0
PE6
Text GLabel 2550 5050 0    50   Input ~ 0
PE7
Text GLabel 4850 3400 0    50   Input ~ 0
WE
Text GLabel 4850 3500 0    50   Input ~ 0
RST
Text GLabel 4850 3800 0    50   Input ~ 0
RY-BY
Text GLabel 5850 4400 2    50   Input ~ 0
OE_FLASH
Text GLabel 2550 5650 0    50   Input ~ 0
RY-BY
Text GLabel 3750 2650 2    50   Input ~ 0
A1
Text GLabel 4850 4700 0    50   Input ~ 0
A1
Text GLabel 6900 3250 0    50   Input ~ 0
A1
Text GLabel 2550 5850 0    50   Input ~ 0
A17
$Comp
L Oscillator:Standard_Clock_Oscillators_XTAL_OSC X1
U 1 1 68CF909B
P 1250 2600
F 0 "X1" H 800 3350 50  0000 L CNN
F 1 "Standard_Clock_Oscillators_XTAL_OSC" H 800 3450 50  0000 L CNN
F 2 "Oscillator:Oscillator_SMD_SeikoEpson_SG8002LB-4Pin_5.0x3.2mm" H 1950 2250 50  0001 C CNN
F 3 "" H 1150 2600 50  0001 C CNN
	1    1250 2600
	1    0    0    -1  
$EndComp
Text GLabel 1250 2300 1    50   Input ~ 0
BOARD_5V
Wire Wire Line
	950  2600 950  2300
Wire Wire Line
	950  2300 1250 2300
Text GLabel 1000 2900 0    50   Input ~ 0
GND
Wire Wire Line
	1000 2900 1250 2900
$Comp
L Device:R_Small R3
U 1 1 68CFCC9A
P 1700 2600
F 0 "R3" V 1800 2600 50  0000 C CNN
F 1 "22" V 1700 2600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 1700 2600 50  0001 C CNN
F 3 "~" H 1700 2600 50  0001 C CNN
	1    1700 2600
	0    1    1    0   
$EndComp
Wire Wire Line
	1550 2600 1600 2600
Wire Wire Line
	1800 2600 2000 2600
Wire Wire Line
	2000 2600 2000 2750
Wire Wire Line
	2000 2750 2550 2750
Text Label 1900 2600 0    50   ~ 0
CLK16M
$Comp
L Device:C_Small C5
U 1 1 68D04809
P 6600 5500
F 0 "C5" H 6692 5546 50  0000 L CNN
F 1 "100nF" H 6692 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 6600 5500 50  0001 C CNN
F 3 "~" H 6600 5500 50  0001 C CNN
	1    6600 5500
	1    0    0    -1  
$EndComp
Text GLabel 5250 5350 0    50   Input ~ 0
BOARD_5V
Text GLabel 5250 5650 0    50   Input ~ 0
GND
Wire Wire Line
	5250 5350 5450 5350
Wire Wire Line
	5450 5350 5450 5400
Wire Wire Line
	5250 5650 5450 5650
Wire Wire Line
	5450 5650 5450 5600
$Comp
L Device:C_Small C1
U 1 1 68D080F7
P 2000 3950
F 0 "C1" H 2092 3996 50  0000 L CNN
F 1 "1uF" H 2092 3905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 2000 3950 50  0001 C CNN
F 3 "~" H 2000 3950 50  0001 C CNN
	1    2000 3950
	1    0    0    -1  
$EndComp
Text GLabel 2000 4050 3    50   Input ~ 0
GND
Wire Wire Line
	2550 3850 2000 3850
$Comp
L Device:C_Small C2
U 1 1 68D18519
P 5450 5500
F 0 "C2" H 5542 5546 50  0000 L CNN
F 1 "100nF" H 5542 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 5450 5500 50  0001 C CNN
F 3 "~" H 5450 5500 50  0001 C CNN
	1    5450 5500
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C3
U 1 1 68D18C10
P 5800 5500
F 0 "C3" H 5892 5546 50  0000 L CNN
F 1 "100nF" H 5892 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 5800 5500 50  0001 C CNN
F 3 "~" H 5800 5500 50  0001 C CNN
	1    5800 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 5400 5800 5350
Wire Wire Line
	5800 5350 5450 5350
Connection ~ 5450 5350
Wire Wire Line
	6200 5400 6200 5350
Wire Wire Line
	6200 5350 5800 5350
Connection ~ 5800 5350
Wire Wire Line
	5450 5650 5800 5650
Wire Wire Line
	5800 5650 5800 5600
Connection ~ 5450 5650
Wire Wire Line
	5800 5650 6200 5650
Wire Wire Line
	6200 5650 6200 5600
Connection ~ 5800 5650
$Comp
L Device:C_Small C4
U 1 1 68D246B1
P 6200 5500
F 0 "C4" H 6292 5546 50  0000 L CNN
F 1 "100nF" H 6292 5455 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.08x0.95mm_HandSolder" H 6200 5500 50  0001 C CNN
F 3 "~" H 6200 5500 50  0001 C CNN
	1    6200 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	6200 5350 6600 5350
Wire Wire Line
	6600 5350 6600 5400
Connection ~ 6200 5350
Wire Wire Line
	6200 5650 6600 5650
Wire Wire Line
	6600 5650 6600 5600
Connection ~ 6200 5650
Text Notes 2550 1350 0    50   ~ 0
G
Text Notes 2900 950  0    50   ~ 0
D
Text Notes 2900 1450 0    50   ~ 0
S
Text Notes 3050 650  0    50   ~ 0
P-Channel MOSFETs
Text GLabel 3250 750  2    50   Input ~ 0
BOARD_5V
$Comp
L Meggy:DMP2305U-7 Q1
U 1 1 68D8212F
P 2550 1250
F 0 "Q1" H 2980 1396 50  0000 L CNN
F 1 "DMP2305U-7" H 2980 1305 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 3000 1200 50  0001 L CNN
F 3 "https://www.diodes.com//assets/Datasheets/DMP2305U.pdf" H 3000 1100 50  0001 L CNN
	1    2550 1250
	1    0    0    -1  
$EndComp
Text Notes 3700 1350 0    50   ~ 0
G
Text Notes 4050 950  0    50   ~ 0
D
Text Notes 4050 1450 0    50   ~ 0
S
$Comp
L Meggy:DMP2305U-7 Q2
U 1 1 68D926EB
P 3700 1250
F 0 "Q2" H 4130 1396 50  0000 L CNN
F 1 "DMP2305U-7" H 4130 1305 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 4150 1200 50  0001 L CNN
F 3 "https://www.diodes.com//assets/Datasheets/DMP2305U.pdf" H 4150 1100 50  0001 L CNN
	1    3700 1250
	1    0    0    -1  
$EndComp
Text GLabel 1700 3250 1    50   Output ~ 0
VBUS_5V
Text GLabel 2850 1550 3    50   Input ~ 0
VBUS_5V
Text GLabel 4000 1550 3    50   Input ~ 0
VCC
Wire Wire Line
	3250 750  3250 850 
Wire Wire Line
	2850 850  3250 850 
Connection ~ 3250 850 
Text GLabel 2550 4450 0    50   Input ~ 0
RST
Text GLabel 2550 4350 0    50   Input ~ 0
WE
Wire Wire Line
	2850 1450 2850 1550
Wire Wire Line
	4000 1450 4000 1550
Text GLabel 2200 1250 0    50   Input ~ 0
GND
Wire Wire Line
	2250 1250 2200 1250
Text GLabel 3500 1600 3    50   Input ~ 0
GND
Wire Wire Line
	3500 1250 3500 1300
Wire Wire Line
	3250 850  4000 850 
$Comp
L Device:D_Small D1
U 1 1 68E1A907
P 2350 1250
F 0 "D1" H 2350 1043 50  0000 C CNN
F 1 "MMDL6050T1G" H 2350 1134 50  0000 C CNN
F 2 "Meggy:D_0603_1608Metric_Pad_0.975x0.95mm_HandSolder" V 2350 1250 50  0001 C CNN
F 3 "~" V 2350 1250 50  0001 C CNN
	1    2350 1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	2450 1250 2550 1250
Wire Wire Line
	3500 1250 3700 1250
$Comp
L Device:D_Small D2
U 1 1 68E38814
P 3500 1400
F 0 "D2" V 3500 1250 50  0000 C CNN
F 1 "MMDL6050T1G" V 3600 1450 50  0000 C CNN
F 2 "Meggy:D_0603_1608Metric_Pad_0.975x0.95mm_HandSolder" V 3500 1400 50  0001 C CNN
F 3 "~" V 3500 1400 50  0001 C CNN
	1    3500 1400
	0    1    1    0   
$EndComp
Wire Wire Line
	3500 1500 3500 1600
$Comp
L Meggy:DMP2305U-7 Q3
U 1 1 68E4900A
P 6100 1500
F 0 "Q3" H 6530 1646 50  0000 L CNN
F 1 "DMP2305U-7" H 6530 1555 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 6550 1450 50  0001 L CNN
F 3 "https://www.diodes.com//assets/Datasheets/DMP2305U.pdf" H 6550 1350 50  0001 L CNN
	1    6100 1500
	1    0    0    -1  
$EndComp
Text GLabel 6400 1000 1    50   Input ~ 0
OE_FLASH
Text Notes 6100 1600 0    50   ~ 0
G
Text Notes 6450 1200 0    50   ~ 0
D
Text Notes 6450 1700 0    50   ~ 0
S
$Comp
L Device:R_Pack04 RN1
U 1 1 68E54917
P 5500 900
F 0 "RN1" H 5688 946 50  0000 L CNN
F 1 "10k" H 5688 855 50  0000 L CNN
F 2 "Meggy:RESCAF80P320X160X60-8N" V 5775 900 50  0001 C CNN
F 3 "~" H 5500 900 50  0001 C CNN
	1    5500 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 1500 6100 1500
$Comp
L Device:D_Small D3
U 1 1 68E5A0C5
P 5300 1650
F 0 "D3" V 5300 1800 50  0000 C CNN
F 1 "MMDL6050T1G" V 5400 2000 50  0000 C CNN
F 2 "Meggy:D_0603_1608Metric_Pad_0.975x0.95mm_HandSolder" V 5300 1650 50  0001 C CNN
F 3 "~" V 5300 1650 50  0001 C CNN
	1    5300 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 1550 5300 1500
Text GLabel 5300 1850 3    50   Input ~ 0
GND
Wire Wire Line
	5300 1850 5300 1750
Text GLabel 5750 650  2    50   Input ~ 0
BOARD_5V
Wire Wire Line
	5400 700  5400 650 
Wire Wire Line
	5400 650  5500 650 
Wire Wire Line
	5500 700  5500 650 
Connection ~ 5500 650 
Wire Wire Line
	5500 650  5600 650 
Wire Wire Line
	5600 700  5600 650 
Connection ~ 5600 650 
Wire Wire Line
	5600 650  5750 650 
Wire Wire Line
	6400 1000 6400 1100
Text GLabel 5600 1100 3    50   Input ~ 0
RESET
Text GLabel 5500 1100 3    50   Input ~ 0
HWB
Text GLabel 5050 650  0    50   Input ~ 0
VBUS_5V
Wire Wire Line
	5300 1500 5150 1500
Connection ~ 5300 1500
Wire Wire Line
	5300 1100 5300 1400
Wire Wire Line
	5300 1400 5800 1400
Wire Wire Line
	5800 1400 5800 1100
Wire Wire Line
	5800 1100 6400 1100
Connection ~ 6400 1100
Wire Wire Line
	5400 650  5300 650 
Wire Wire Line
	5300 650  5300 700 
Connection ~ 5400 650 
Text GLabel 5400 1100 3    50   Input ~ 0
BYTE
$Comp
L Device:R_Small R5
U 1 1 68EC4E2C
P 5150 900
F 0 "R5" H 5300 850 50  0000 C CNN
F 1 "10k" H 5300 950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 5150 900 50  0001 C CNN
F 3 "~" H 5150 900 50  0001 C CNN
	1    5150 900 
	-1   0    0    1   
$EndComp
Wire Wire Line
	5050 650  5150 650 
Wire Wire Line
	5150 650  5150 800 
Wire Wire Line
	5150 1000 5150 1500
Wire Wire Line
	1700 3250 1700 3350
Connection ~ 1700 3350
Wire Wire Line
	1700 3350 1850 3350
Text Label 2200 3850 0    50   ~ 0
UCAP
Text GLabel 2550 5550 0    50   Input ~ 0
OE_FLASH
Text GLabel 6400 1700 3    50   Input ~ 0
OE_SOCKET
Text GLabel 6900 3650 0    50   Input ~ 0
OE_SOCKET
$EndSCHEMATC

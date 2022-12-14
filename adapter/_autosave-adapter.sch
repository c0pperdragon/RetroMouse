EESchema Schematic File Version 5
EELAYER 36 0
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
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
Connection ~ 2850 3900
Wire Wire Line
	2750 3900 2750 3850
Wire Wire Line
	2850 3850 2850 3900
Wire Wire Line
	2850 3900 2750 3900
Wire Wire Line
	2850 3900 2850 3950
Wire Wire Line
	3150 3250 3200 3250
Wire Wire Line
	3150 3450 4700 3450
Wire Wire Line
	3450 3350 4700 3350
Wire Wire Line
	3450 3550 3150 3550
Wire Wire Line
	3450 3550 3450 3350
Wire Wire Line
	4000 5300 4000 5400
Wire Wire Line
	4000 5600 4000 5700
Wire Wire Line
	5100 5300 5200 5300
Wire Wire Line
	5200 5500 4850 5500
Wire Wire Line
	5300 2800 5300 2850
Wire Wire Line
	5300 4250 5300 4300
Wire Wire Line
	5900 3850 8150 3850
Wire Wire Line
	6950 3150 8150 3150
Wire Wire Line
	8150 3250 7250 3250
Wire Wire Line
	8150 3350 7050 3350
Wire Wire Line
	8150 3450 8100 3450
Wire Wire Line
	8150 3550 7150 3550
Wire Wire Line
	8150 3650 8100 3650
Wire Wire Line
	8150 3750 7150 3750
Text Label 4250 3350 0    50   ~ 0
DATA
Text Label 4250 3450 0    50   ~ 0
CLOCK
Text Label 4850 5500 0    50   ~ 0
UDPI
$Comp
L power:+5V #PWR0104
U 1 1 63231014
P 3200 3250
F 0 "#PWR0104" H 3200 3100 50  0001 C CNN
F 1 "+5V" V 3215 3378 50  0000 L CNN
F 2 "" H 3200 3250 50  0001 C CNN
F 3 "" H 3200 3250 50  0001 C CNN
	1    3200 3250
	0    1    1    0   
$EndComp
$Comp
L power:+5V #PWR0107
U 1 1 632313B1
P 4000 5300
F 0 "#PWR0107" H 4000 5150 50  0001 C CNN
F 1 "+5V" V 4015 5428 50  0000 L CNN
F 2 "" H 4000 5300 50  0001 C CNN
F 3 "" H 4000 5300 50  0001 C CNN
	1    4000 5300
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 00000000
P 5300 2800
F 0 "#PWR?" H 5300 2650 50  0001 C CNN
F 1 "+5V" V 5315 2928 50  0000 L CNN
F 2 "" H 5300 2800 50  0001 C CNN
F 3 "" H 5300 2800 50  0001 C CNN
	1    5300 2800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0106
U 1 1 63231208
P 8100 3450
F 0 "#PWR0106" H 8100 3300 50  0001 C CNN
F 1 "+5V" V 8115 3578 50  0000 L CNN
F 2 "" H 8100 3450 50  0001 C CNN
F 3 "" H 8100 3450 50  0001 C CNN
	1    8100 3450
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 63230FB7
P 2850 3950
F 0 "#PWR0102" H 2850 3700 50  0001 C CNN
F 1 "GND" H 2855 3777 50  0000 C CNN
F 2 "" H 2850 3950 50  0001 C CNN
F 3 "" H 2850 3950 50  0001 C CNN
	1    2850 3950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0108
U 1 1 632313EE
P 4000 5700
F 0 "#PWR0108" H 4000 5450 50  0001 C CNN
F 1 "GND" V 4005 5572 50  0000 R CNN
F 2 "" H 4000 5700 50  0001 C CNN
F 3 "" H 4000 5700 50  0001 C CNN
	1    4000 5700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 6323DE96
P 5100 5300
F 0 "#PWR0109" H 5100 5050 50  0001 C CNN
F 1 "GND" V 5105 5172 50  0000 R CNN
F 2 "" H 5100 5300 50  0001 C CNN
F 3 "" H 5100 5300 50  0001 C CNN
	1    5100 5300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 00000000
P 5300 4300
F 0 "#PWR?" H 5300 4050 50  0001 C CNN
F 1 "GND" V 5305 4172 50  0000 R CNN
F 2 "" H 5300 4300 50  0001 C CNN
F 3 "" H 5300 4300 50  0001 C CNN
	1    5300 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 632311D0
P 8100 3650
F 0 "#PWR0105" H 8100 3400 50  0001 C CNN
F 1 "GND" V 8105 3522 50  0000 R CNN
F 2 "" H 8100 3650 50  0001 C CNN
F 3 "" H 8100 3650 50  0001 C CNN
	1    8100 3650
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C1
U 1 1 63231360
P 4000 5500
F 0 "C1" V 4229 5500 50  0000 C CNN
F 1 "10uF" V 4138 5500 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4000 5500 50  0001 C CNN
F 3 "~" H 4000 5500 50  0001 C CNN
	1    4000 5500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 6323DD88
P 5400 5300
F 0 "J3" H 5480 5342 50  0000 L CNN
F 1 "Conn_01x01" H 5480 5251 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 5400 5300 50  0001 C CNN
F 3 "~" H 5400 5300 50  0001 C CNN
	1    5400 5300
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 6323DDFF
P 5400 5500
F 0 "J4" H 5480 5542 50  0000 L CNN
F 1 "Conn_01x01" H 5480 5451 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x01_P2.54mm_Vertical" H 5400 5500 50  0001 C CNN
F 3 "~" H 5400 5500 50  0001 C CNN
	1    5400 5500
	1    0    0    -1  
$EndComp
$Comp
L adapter-rescue:USB_A-Connector J2
U 1 1 63230EEC
P 2850 3450
F 0 "J2" H 2620 3348 50  0000 R CNN
F 1 "USB_A" H 2620 3439 50  0000 R CNN
F 2 "Connector_USB:USB_A_CNCTech_1001-011-01101_Horizontal" H 3000 3400 50  0001 C CNN
F 3 " ~" H 3000 3400 50  0001 C CNN
	1    2850 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector:DB9_Female J1
U 1 1 63231169
P 8450 3550
F 0 "J1" H 8370 2858 50  0000 C CNN
F 1 "DB9_Female" H 8370 2949 50  0000 C CNN
F 2 "Connector_Dsub:DSUB-9_Female_EdgeMount_P2.77mm" H 8450 3550 50  0001 C CNN
F 3 " ~" H 8450 3550 50  0001 C CNN
	1    8450 3550
	1    0    0    -1  
$EndComp
$Comp
L MCU_Microchip_ATtiny:ATtiny1604-SS U?
U 1 1 00000000
P 5300 3550
F 0 "U?" H 5380 4350 50  0000 L CNN
F 1 "ATtiny1604-SS" H 5380 4250 50  0000 L CNN
F 2 "Package_SO:SOIC-14_3.9x8.7mm_P1.27mm" H 5300 3550 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny804_1604-Data-Sheet-40002028A.pdf" H 5300 3550 50  0001 C CNN
	1    5300 3550
	1    0    0    -1  
$EndComp
$EndSCHEMATC

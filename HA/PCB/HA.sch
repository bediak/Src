EESchema Schematic File Version 4
LIBS:HA-cache
EELAYER 26 0
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
L ATMEGA328P-AU:ATMEGA328P-AU IC?
U 1 1 5B85BC19
P 3000 3200
F 0 "IC?" H 3950 3500 50  0000 L CNN
F 1 "ATMEGA328P-AU" H 3750 2300 50  0000 L CNN
F 2 "QFP80P900X900X120-32N" H 3000 3200 50  0001 L BNN
F 3 "Microchip" H 3000 3200 50  0001 L BNN
F 4 "" H 3000 3200 50  0001 L BNN "Pole4"
F 5 "RS" H 3000 3200 50  0001 L BNN "Pole5"
F 6 "" H 3000 3200 50  0001 L BNN "Pole6"
F 7 "1310271" H 3000 3200 50  0001 L BNN "Pole7"
F 8 "1.2mm" H 3000 3200 50  0001 L BNN "Pole8"
F 9 "ATMEGA328P-AU" H 3000 3200 50  0001 L BNN "Pole9"
F 10 "" H 3000 3200 50  0001 L BNN "Pole10"
F 11 "MICROCHIP - ATMEGA328P-AU. - MICROCONTROLLER MCU, 8 BIT, ATMEGA, 20MHZ, TQFP-32" H 3000 3200 50  0001 L BNN "Pole11"
	1    3000 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B85BD38
P 2350 3400
F 0 "C?" H 2442 3446 50  0000 L CNN
F 1 "100nF" H 2442 3355 50  0000 L CNN
F 2 "" H 2350 3400 50  0001 C CNN
F 3 "~" H 2350 3400 50  0001 C CNN
	1    2350 3400
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B85BDBD
P 2350 3700
F 0 "C?" H 2442 3746 50  0000 L CNN
F 1 "100nF" H 2442 3655 50  0000 L CNN
F 2 "" H 2350 3700 50  0001 C CNN
F 3 "~" H 2350 3700 50  0001 C CNN
	1    2350 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3500 2350 3500
Wire Wire Line
	2350 3500 2100 3500
Connection ~ 2350 3500
Wire Wire Line
	2350 3300 2750 3300
Wire Wire Line
	2750 3300 2750 3400
Wire Wire Line
	2750 3400 3000 3400
Wire Wire Line
	2350 3300 2100 3300
Connection ~ 2350 3300
Wire Wire Line
	2350 3600 2100 3600
Wire Wire Line
	2350 3600 3000 3600
Connection ~ 2350 3600
Wire Wire Line
	3000 3700 2750 3700
Wire Wire Line
	2750 3700 2750 3800
Wire Wire Line
	2750 3800 2350 3800
Wire Wire Line
	2350 3800 2100 3800
Connection ~ 2350 3800
$Comp
L Device:Crystal Y?
U 1 1 5B85C3B9
P 2750 4300
F 0 "Y?" H 2750 4568 50  0000 C CNN
F 1 "Crystal" H 2750 4477 50  0000 C CNN
F 2 "" H 2750 4300 50  0001 C CNN
F 3 "~" H 2750 4300 50  0001 C CNN
	1    2750 4300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 3900 3000 4300
Wire Wire Line
	3000 4300 2900 4300
Wire Wire Line
	2900 4300 2750 4300
Wire Wire Line
	3000 3800 2850 3800
Wire Wire Line
	2850 3800 2850 3900
Wire Wire Line
	2850 3900 2600 3900
Wire Wire Line
	2600 3900 2600 4300
Wire Wire Line
	2600 4300 2600 4500
Connection ~ 2600 4300
Wire Wire Line
	2900 4300 2900 4500
$Comp
L Device:C_Small C?
U 1 1 5B85CB31
P 2600 4600
F 0 "C?" H 2400 4650 50  0000 L CNN
F 1 "22pF" H 2300 4550 50  0000 L CNN
F 2 "" H 2600 4600 50  0001 C CNN
F 3 "~" H 2600 4600 50  0001 C CNN
	1    2600 4600
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B85CBBB
P 2900 4600
F 0 "C?" H 2992 4646 50  0000 L CNN
F 1 "22pF" H 2992 4555 50  0000 L CNN
F 2 "" H 2900 4600 50  0001 C CNN
F 3 "~" H 2900 4600 50  0001 C CNN
	1    2900 4600
	1    0    0    -1  
$EndComp
Connection ~ 2900 4300
Wire Wire Line
	2600 4700 2600 4800
Wire Wire Line
	2900 4800 2900 4700
Wire Wire Line
	2600 4800 2750 4800
Wire Wire Line
	2750 4800 2750 4900
Connection ~ 2750 4800
Wire Wire Line
	2750 4800 2900 4800
$Comp
L power:GND #PWR?
U 1 1 5B85D8A8
P 2750 4900
F 0 "#PWR?" H 2750 4650 50  0001 C CNN
F 1 "GND" H 2755 4727 50  0000 C CNN
F 2 "" H 2750 4900 50  0001 C CNN
F 3 "" H 2750 4900 50  0001 C CNN
	1    2750 4900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5B85D8C6
P 2100 3600
F 0 "#PWR?" H 2100 3350 50  0001 C CNN
F 1 "GND" H 1950 3500 50  0000 C CNN
F 2 "" H 2100 3600 50  0001 C CNN
F 3 "" H 2100 3600 50  0001 C CNN
	1    2100 3600
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5B85D8F9
P 2100 3300
F 0 "#PWR?" H 2100 3050 50  0001 C CNN
F 1 "GND" H 2250 3250 50  0000 C CNN
F 2 "" H 2100 3300 50  0001 C CNN
F 3 "" H 2100 3300 50  0001 C CNN
	1    2100 3300
	-1   0    0    1   
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B85D910
P 5450 3500
F 0 "C?" H 5542 3546 50  0000 L CNN
F 1 "100nF" H 5542 3455 50  0000 L CNN
F 2 "" H 5450 3500 50  0001 C CNN
F 3 "~" H 5450 3500 50  0001 C CNN
	1    5450 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C_Small C?
U 1 1 5B85D946
P 5450 3900
F 0 "C?" H 5542 3946 50  0000 L CNN
F 1 "100nF" H 5542 3855 50  0000 L CNN
F 2 "" H 5450 3900 50  0001 C CNN
F 3 "~" H 5450 3900 50  0001 C CNN
	1    5450 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3400 5900 3400
$Comp
L power:GND #PWR?
U 1 1 5B85E455
P 5900 3400
F 0 "#PWR?" H 5900 3150 50  0001 C CNN
F 1 "GND" H 6050 3350 50  0000 C CNN
F 2 "" H 5900 3400 50  0001 C CNN
F 3 "" H 5900 3400 50  0001 C CNN
	1    5900 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 3800 5000 3800
Wire Wire Line
	5200 3500 5200 3400
Wire Wire Line
	5200 3400 5450 3400
Wire Wire Line
	5000 3500 5200 3500
Connection ~ 5450 3400
Wire Wire Line
	5000 3600 5450 3600
Wire Wire Line
	5450 3600 5900 3600
Connection ~ 5450 3600
$Comp
L power:+5V #PWR?
U 1 1 5B861BA1
P 5900 3600
F 0 "#PWR?" H 5900 3450 50  0001 C CNN
F 1 "+5V" H 5915 3773 50  0000 C CNN
F 2 "" H 5900 3600 50  0001 C CNN
F 3 "" H 5900 3600 50  0001 C CNN
	1    5900 3600
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5B862417
P 2100 3800
F 0 "#PWR?" H 2100 3650 50  0001 C CNN
F 1 "+5V" H 2250 3900 50  0000 C CNN
F 2 "" H 2100 3800 50  0001 C CNN
F 3 "" H 2100 3800 50  0001 C CNN
	1    2100 3800
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5B862432
P 2100 3500
F 0 "#PWR?" H 2100 3350 50  0001 C CNN
F 1 "+5V" H 1950 3550 50  0000 C CNN
F 2 "" H 2100 3500 50  0001 C CNN
F 3 "" H 2100 3500 50  0001 C CNN
	1    2100 3500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5B862FD7
P 5450 4000
F 0 "#PWR?" H 5450 3750 50  0001 C CNN
F 1 "GND" H 5600 3950 50  0000 C CNN
F 2 "" H 5450 4000 50  0001 C CNN
F 3 "" H 5450 4000 50  0001 C CNN
	1    5450 4000
	1    0    0    -1  
$EndComp
$EndSCHEMATC

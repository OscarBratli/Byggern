# ATmega162 Pin Mapping and Connections

Back to [README](README.md)

## Overview
This document tracks all pin connections between ATmega162, MAX156 ADC, and joystick board.

---

## ATmega162 Pin Assignments

### Port A (PA0-PA7) - Data Bus
**Used for: External memory bus (SRAM + ADC shared data lines)**
```
Pin | ATmega162 | Connection
----|-----------|------------
40  | PA0       | Data Bus D0 (shared: SRAM + MAX156)
39  | PA1       | Data Bus D1 (shared: SRAM + MAX156)
38  | PA2       | Data Bus D2 (shared: SRAM + MAX156)
37  | PA3       | Data Bus D3 (shared: SRAM + MAX156)
36  | PA4       | Data Bus D4 (shared: SRAM + MAX156)
35  | PA5       | Data Bus D5 (shared: SRAM + MAX156)
34  | PA6       | Data Bus D6 (shared: SRAM + MAX156)
33  | PA7       | Data Bus D7 (shared: SRAM + MAX156)
```

### Port B (PB0-PB7) - SPI + Control
```
Pin | ATmega162 | Function | Connection
----|-----------|----------|------------
1   | PB0       | GPIO     | Available
2   | PB1       | GPIO     | Available  
3   | PB2       | GPIO     | Joystick DISP_D/C (Pin 0)
4   | PB3       | GPIO     | Joystick DISP_CS (Pin 1)
5   | PB4       | GPIO/SS  | Joystick IO_CS (Pin 2)
6   | PB5       | MOSI     | Joystick MOSI (Pin 3)
7   | PB6       | MISO     | Joystick MISO (Pin 4)
8   | PB7       | SCK      | Joystick SCK (Pin 5)
```

### Port C (PC0-PC7) - Address + Control + JTAG
```
Pin | ATmega162 | Function | Connection
----|-----------|----------|------------
21  | PC0       | GPIO     | Available
22  | PC1       | GPIO     | Available
23  | PC2       | A10      | Address decoder (NAND gate input)
24  | PC3       | A11      | Address decoder (NAND gate input)
25  | PC4       | TDO      | JTAG - Test Data Output (RESERVED)
26  | PC5       | TDI      | JTAG - Test Data Input (RESERVED)
27  | PC6       | TMS      | JTAG - Test Mode Select (RESERVED)
28  | PC7       | TCK      | JTAG - Test Clock (RESERVED)
```

### Port D (PD0-PD7) - Mixed I/O
```
Pin | ATmega162 | Function | Connection
----|-----------|----------|------------
10  | PD0       | GPIO     | Available
11  | PD1       | GPIO     | Available
12  | PD2       | GPIO     | Available
13  | PD3       | GPIO     | Available
14  | PD4       | OC1A     | MAX156 CLK (Pin 9) - Timer1 output
15  | PD5       | GPIO     | Available
16  | PD6       | GPIO     | Available
17  | PD7       | GPIO     | Available
```

### Other Important Pins
```
Pin | ATmega162 | Function | Connection
----|-----------|----------|------------
31  | WR        | Write    | MAX156 WR + SRAM WR
32  | RD        | Read     | MAX156 RD + SRAM RD
```

---

## MAX156 ADC Connections

### Power & Reference
```
MAX156 Pin | Function | Connection
-----------|----------|------------
14         | VDD      | +5V
7          | VSS      | GND
13         | AGND     | GND
4          | REFIN    | +5V (external reference)
3          | REFOUT   | Not connected (using external ref)
```

### Data Interface
```
MAX156 Pin | Function | Connection
-----------|----------|------------
18         | D0       | Shared data bus D0
17         | D1       | Shared data bus D1
16         | D2       | Shared data bus D2
15         | D3       | Shared data bus D3
14         | D4       | Shared data bus D4
13         | D5       | Shared data bus D5
12         | D6       | Shared data bus D6
11         | D7       | Shared data bus D7
```

### Control Signals
```
MAX156 Pin | Function | Connection
-----------|----------|------------
11         | CS       | Address decoder output (from NAND gate)
6          | RD       | ATmega162 RD (Pin 32)
5          | WR       | ATmega162 WR (Pin 31)
9          | CLK      | ATmega162 PD4 (Timer1 OC1A output)
8          | BUSY     | Not connected (optional)
```

### Analog Inputs
```
MAX156 Pin | Function | Connection
-----------|----------|------------
1          | AIN0     | Joystick JOY_Y (Pin 13)
2          | AIN1     | Joystick JOY_X (Pin 12)
24         | AIN2     | Touch PAD_Y (Pin 10)
23         | AIN3     | Joystick PAD_X (Pin 11)
```

---

## Joystick Board Connections

### Digital Signals to ATmega162
```
Joystick Pin | Function  | ATmega162 Connection
-------------|-----------|--------------------
0            | DISP_D/C  | PB2 (Pin 3)
1            | DISP_CS   | PB3 (Pin 4)
2            | IO_CS     | PB4 (Pin 5)
3            | MOSI      | PB5 (Pin 6)
4            | MISO      | PB6 (Pin 7)
5            | SCK       | PB7 (Pin 8)
9            | JOY_B     | Available GPIO (PC0, PC1, PD0-PD3, PD5-PD7)
```

### Analog Signals to MAX156
```
Joystick Pin | Function | MAX156 Connection
-------------|----------|------------------
10           | PAD_Y    | AIN2 (Pin 24)
11           | PAD_X    | AIN3 (Pin 23)
12           | JOY_Y    | AIN1 (Pin 2)
13           | JOY_X    | AIN0 (Pin 1)
```

### Power
```
Joystick Pin | Function | Connection
-------------|----------|------------
7            | GND      | System Ground
8            | 5V       | System +5V
```

---

## Address Decoder Logic

### NAND Gate Configuration
```
Input A: PC3 (A11)
Input B: PC2 (A10)
Output:  SRAM CS

Truth Table:
A11 | A10 | NAND Output | Selected Device
----|-----|-------------|----------------
 0  |  0  |      1      | ADC (SRAM not selected)
 0  |  1  |      0      | SRAM
 1  |  0  |      0      | SRAM
 1  |  1  |      0      | SRAM
```


#### JTAG Pins 


### Memory Map
```
Address Range    | Device Selected
-----------------|----------------
0x1000 - 0x13FF  | MAX156 ADC
0x1400 - 0x17FF  | SRAM
0x1800 - 0x1BFF  | SRAM
0x1C00 - 0x1FFF  | SRAM
```

---

## Software Usage Examples

### ADC Reading
```c
// Read joystick X (channel 0)
volatile uint8_t* adc_ptr = (uint8_t*)0x1000;
uint8_t joy_x = *adc_ptr;

// Read joystick Y (channel 1)  
// (Need to set channel selection first)
```

### SPI Communication
```c
// Select joystick board
PORTB &= ~(1 << PB4);  // IO_CS low

// Select display
PORTB &= ~(1 << PB3);  // DISP_CS low
PORTB &= ~(1 << PB2);  // DISP_D/C low (command mode)
```

---

## Notes
- All data lines (D0-D7) are shared between SRAM and MAX156
- Only one device can drive the bus at a time (controlled by CS)
- Timer1 generates continuous clock for MAX156 on PD4
- SPI is used only for joystick board communication (display, etc.)
- ADC uses parallel/memory-mapped interface, not SPI
- **JTAG pins PC4-PC7 are RESERVED** for programming/debugging - do not use for GPIO
- Available GPIO pins for additional connections: PC0, PC1, PD0-PD3, PD5-PD7, PB0, PB1

Last updated: October 1, 2025
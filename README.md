# Byggern

File Repository for Byggern Course at NTNU
Here is how the branching and forking should be done, to ensure a clean repository

[wiring_tables.md](wiring_tables.md) contains lists of the pin connections.

## Installation

The steps below are explained in more detail in page 22 of the [lab manual](lab_manual.pdf)

### Install compiler

```
sudo apt install gcc-arm-none-eabi
```

### Install prerequisites for a newer version of openocd

```
sudo apt install libusb-dev libusb-1.0-0-dev libusb-1.0-0
```

### Install Open-jtag dependencies

```
sudo apt install libhidapi-dev libftdi-dev libftdi1-dev
```

### Install additional dependencies

These are likely required for building openocd on Ubuntu in WSL

```
sudo apt install pkg-config libjim-dev
```

### Install avr toolchain

```
sudo apt install gcc-avr binutils-avr avr-libc
```

### Install and build openocd

```
git clone https://git.code.sf.net/p/openocd/code openocd
cd openocd
./bootstrap
mkdir build; cd build
../configure --enable-cmsis-dap --enable-openjtag --prefix=/opt/openocd
make
sudo make install
```

## Build project

```
make
```

## Flash device

```
make flash
```

## Serial port

### Ubuntu

Run the following command, where `/dev/ttyS0` is the device and `9600` is the baud rate in a terminal to open the serial communication

```
sudo screen /dev/ttyS0 9600
```

#### Kill serial port

If the serial port is started with the command above, there is a chance it was not gracefully shut down, and the screen instance is still watching the port. This might interfere with the operation of the serial port and data may be lost. To fix this, check if there are any applications currently using the serial port using this command.

```
sudo lsof /dev/ttyS0
```

And kill the applications listed.

```
sudo pkill screen
```

## OLED Display Driver

This project includes a complete SSD1306 OLED display driver with SPI communication.

### Hardware Configuration

**OLED Control Pins:**
- `OLED_DC (PB2)` - Data/Command pin
- `OLED_CS (PB3)` - Chip Select pin  
- `OLED_RES (PD5)` - Reset pin

**SPI Pins:**
- `SPI_MOSI (PB5)` - Master Out Slave In
- `SPI_SCK (PB7)` - Serial Clock
- `SPI_MISO (PB6)` - Master In Slave Out (not used for OLED)

### OLED Commands Used

Our implementation follows the lab manual recommendations for **minimal initialization** and uses **Page Mode** addressing:

#### Initialization Commands
```c
0xA1    // Segment remap (flip horizontally)
0xC8    // COM scan direction (flip vertically)  
0xAF    // Display ON
```

#### Addressing Commands  
```c
0x20    // Set Memory Addressing Mode
0x10    // Page Addressing Mode (default, easiest to use)
0xB0+n  // Set Page Address (0-7, where n is page number)
0x00+n  // Set Lower Column Address (0x00-0x0F)
0x10+n  // Set Higher Column Address (0x10-0x1F)
```

#### Display Commands
```c
0xAE    // Display OFF
0xAF    // Display ON
0x81    // Set Contrast Control
0xA4    // Entire Display ON (resume from RAM)
0xA6    // Normal Display (not inverted)
0x8D    // Charge Pump Setting
0x14    // Enable Charge Pump
```

### Usage Examples

#### Basic Usage
```c
#include "oled/oled.h"

// Initialize OLED display
oled_init();

// Clear the screen (turn all pixels black)
oled_clear_screen();

// Display text
oled_print_string("Hello World!", 0, 2);

// Fill entire screen white
oled_fill_screen_white();
```

#### Page Mode Positioning
```c
// Set cursor to page 3, column 16
oled_set_page_cursor(3, 16);

// Print character at specific position  
oled_print_char('A', 32, 1);  // x=32, page=1
```

### Memory Layout

The OLED uses **Page Mode** addressing:
- **128 columns** (0-127) horizontally
- **8 pages** (0-7) vertically  
- Each page represents **8 vertical pixels**
- Total display: **128x64 pixels**

```
Page 0: [8 pixel rows]  ← Top of screen
Page 1: [8 pixel rows]
Page 2: [8 pixel rows]
Page 3: [8 pixel rows]  ← Middle of screen  
Page 4: [8 pixel rows]
Page 5: [8 pixel rows]
Page 6: [8 pixel rows]
Page 7: [8 pixel rows]  ← Bottom of screen
```

### API Reference

| Function | Description |
|----------|-------------|
| `oled_init()` | Initialize OLED with minimal setup (A1, C8, AF) |
| `oled_clear_screen()` | Clear entire screen (black) |
| `oled_fill_screen_white()` | Fill entire screen white |
| `oled_print_char(c, x, y)` | Print single character at position |
| `oled_print_string(str, x, y)` | Print string starting at position |
| `oled_set_page_cursor(page, col)` | Set cursor position in page mode |
| `oled_write_command(cmd)` | Send command to OLED |
| `oled_write_data(data)` | Send data byte to OLED |

### Font System

Text rendering uses the **8x8 pixel font** from `src/fonts/fonts.h`:
- **95 printable ASCII characters** (32-126)
- **8 pixels wide, 8 pixels tall** per character
- **Stored in program memory** (PROGMEM) to save RAM

### SPI Communication

The OLED communicates via **SPI Mode 0**:
- **CPOL = 0** (Clock polarity)
- **CPHA = 0** (Clock phase)  
- **MSB first** data transmission
- **Clock rate**: fck/16 (configurable)

## Joystick and Slider Driver

This project includes a calibrated joystick and touchpad slider driver that converts ADC readings to normalized position values.

### Hardware Configuration

**ADC Channels:**
- `A0` - Joystick Y axis
- `A1` - Joystick X axis  
- `A2` - Touchpad X axis
- `A3` - Touchpad Y axis

### Calibration Data and Voltage-ADC Relationships

Based on measured voltages and corresponding ADC values:

#### Joystick Y Axis (A0)
```
Center: 2.472V → ADC 160
Range:  1.165V - 3.857V → ADC 80 - 240
Formula: ADC = (Voltage - 1.165) × (160)/(2.692) + 80
Linear relationship: ~59.4 ADC units per volt
```

#### Joystick X Axis (A1)  
```
Center: 2.505V → ADC 160
Range:  0.934V - 4.080V → ADC 61 - 251  
Formula: ADC = (Voltage - 0.934) × (190)/(3.146) + 61
Linear relationship: ~60.4 ADC units per volt
```

#### Touchpad X Axis (A2)
```
Center: 2.505V → ADC 159
Range:  0.128V - 4.967V → ADC 7 - 255
Formula: ADC = (Voltage - 0.128) × (248)/(4.839) + 7
Linear relationship: ~51.3 ADC units per volt
```

#### Touchpad Y Axis (A3)
```
Center: 2.850V → ADC 181  
Range:  0.012V - 4.967V → ADC 0 - 255
Formula: ADC = (Voltage - 0.012) × (255)/(4.955) + 0
Linear relationship: ~51.5 ADC units per volt
```

### Driver Features

**Joystick (`joystick_pos_t`):**
- **Normalized output**: -100 to +100 for each axis
- **Deadzone**: ±5 ADC units around center (eliminates jitter)
- **Center calibration**: Auto-centers at measured center points
- **Button support**: Ready for future button implementation

**Touchpad (`slider_pos_t`):**
- **Full range output**: 0 to 255 for each axis
- **Touch detection**: Automatic detection when touchpad is being used
- **Edge handling**: Proper clamping at min/max values

### Usage Example

```c
#include "joystick_driver/joystick_driver.h"

// Initialize
joystick_driver_init();

// Read joystick position
joystick_pos_t joy = joystick_get_position();
printf("Joystick: X=%d, Y=%d\n", joy.x, joy.y);  // -100 to +100

// Read touchpad position  
slider_pos_t slider = slider_get_position();
printf("Touchpad: X=%d, Y=%d, Touched=%d\n", 
       slider.x, slider.y, slider.touched);  // 0 to 255
```

### Calibration Constants

All calibration values are defined in `joystick_driver.h`:
```c
#define JOYSTICK_X_CENTER   160     // Measured center point
#define JOYSTICK_Y_CENTER   160     
#define JOYSTICK_DEADZONE   5       // Deadzone radius
// ... (see header file for complete calibration data)
```

## Specify microcontroller

### IDE

Include a definition in `.vscode/c_cpp_properties.json` such that the IDE can recognize the IO config

```json
"defines": [
    "__AVR_ATmega162__"
],
```

### CMake

Specify the following in the Makefile such that the compiler can recognize the IO config

```
TARGET_CPU := atmega162
TARGET_DEVICE := m162
```

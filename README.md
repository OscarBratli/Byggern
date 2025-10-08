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

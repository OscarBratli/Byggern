# Byggern
File Repositry for Byggern Course at NTNU
Here is how the branching and forking should be done, to ensure a clean repositry 

## Installation

Clone openocd repo:

```

```

## Build project

```
make
```

## Flash device

```
make flash
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

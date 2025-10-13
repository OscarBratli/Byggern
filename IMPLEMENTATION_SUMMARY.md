# Joystick and Slider Driver - Implementation Summary

## Overview

This implementation provides a complete software driver for reading joystick and slider positions from the MAX156 ADC. The driver is designed to be consistent with the values displayed on the OLED screen.

## Files Created

### Core Driver Files
1. **src/driver/driver.h** - Main driver interface
2. **src/driver/driver.c** - Main driver implementation
3. **src/slider/slider.h** - Slider structure and functions
4. **src/slider/slider.c** - Slider implementation
5. **src/joystick/joystick.h** - Enhanced with ADC support
6. **src/joystick/joystick.c** - Enhanced with ADC support

### Test and Example Files
7. **src/test/driver/driver.h** - Driver test interface
8. **src/test/driver/driver.c** - Driver test implementation
9. **src/main_driver_example.c** - Complete example with OLED display

### Documentation
10. **DRIVER_README.md** - Comprehensive usage documentation

## Key Features

### 1. ADC-Based Joystick Support
- New `AdcJoystick` type that reads directly from ADC channels
- Configurable min/max ADC values for accurate calibration
- Support for deadzone and rest position calibration
- Returns position in range [-1.0, 1.0] for both axes

### 2. Slider/Touchpad Support
- New `Slider` type for touchpad input
- Configurable min/max ADC values
- Returns position in range [0.0, 1.0] for both axes
- Simple and efficient API

### 3. Unified Driver Interface
- Single initialization: `driver_init()`
- Easy reading: `driver_read_joystick()` and `driver_read_slider()`
- Access to raw ADC values: `driver_read_raw_adc()`
- Access to underlying structures for advanced usage

### 4. Accurate Calibration
Based on the measured hardware values:
- Joystick X: Min=61, Center=160, Max=251
- Joystick Y: Min=80, Center=160, Max=240
- Slider X: Min=7, Center=159, Max=255
- Slider Y: Center=181 (min/max estimated)

## Voltage to ADC Relationship

The driver documentation includes the mathematical relationship between voltage and ADC values:

### Linear Relationship
```
ADC = (V - V_min) / (V_max - V_min) * (ADC_max - ADC_min) + ADC_min
```

### Specific Formulas
- **Joystick X**: `ADC ≈ 60.4 * V - 56.4`
- **Joystick Y**: `ADC ≈ 59.4 * V + 10.8`
- **Slider X**: `ADC ≈ 51.2 * V + 0.5`

These formulas are documented in the driver header files and README.

## Usage Example

```c
#include "driver/driver.h"

// Initialize
driver_init();

// Read positions
Vec2 joy = driver_read_joystick();    // Returns [-1.0, 1.0]
Vec2 slider = driver_read_slider();   // Returns [0.0, 1.0]

// Read raw ADC
uint8_t jx, jy, sx, sy;
driver_read_raw_adc(&jx, &jy, &sx, &sy);

// Display
printf("Joystick: X=%.2f Y=%.2f (ADC: %d,%d)\n", 
       joy.x, joy.y, jx, jy);
printf("Slider: X=%.2f Y=%.2f (ADC: %d,%d)\n", 
       slider.x, slider.y, sx, sy);
```

## Integration with OLED

The `main_driver_example.c` shows how to display joystick and slider values on the OLED screen, demonstrating that the driver returns values consistent with the display.

## Testing

To test the driver:

1. **Using the test module**:
   ```c
   #include "test/driver/driver.h"
   driver_test_setup();
   while(1) driver_test_loop();
   ```

2. **Using the example main**:
   - Replace `src/main.c` with `src/main_driver_example.c` in the build
   - Flash to hardware
   - Observe values on OLED and via UART

## Architecture

```
driver.c/h (High-level API)
    |
    +-- joystick.c/h (Joystick with ADC support)
    |       |
    |       +-- adc.c/h (ADC reading)
    |       +-- scale.c/h (Value scaling)
    |       +-- vec2.c/h (2D vectors)
    |
    +-- slider.c/h (Slider/touchpad)
            |
            +-- scale.c/h (Value scaling)
            +-- vec2.c/h (2D vectors)
```

## Minimal Changes

The implementation follows minimal-change principles:
1. Extended existing joystick module rather than replacing it
2. Created new slider module following existing patterns
3. Added unified driver interface for convenience
4. No changes to existing ADC, scale, or vec2 modules
5. Documented voltage-to-ADC relationships as requested

## Next Steps

The driver is ready for hardware testing. To use it:
1. Include `driver/driver.h` in your application
2. Call `driver_init()` in setup
3. Call `driver_read_joystick()` and `driver_read_slider()` to get positions
4. Verify values match OLED display expectations

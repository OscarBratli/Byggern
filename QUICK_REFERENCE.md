# Quick Reference - Joystick and Slider Driver

## Hardware Setup
```
ADC Channel 0 → Joystick Y
ADC Channel 1 → Joystick X
ADC Channel 2 → Slider X
ADC Channel 3 → Slider Y
```

## Calibration Values (Measured from Hardware)

| Component   | Min ADC | Center ADC | Max ADC | Min Voltage | Center Voltage | Max Voltage |
|-------------|---------|------------|---------|-------------|----------------|-------------|
| Joystick X  | 61      | 160        | 251     | 0.934V      | 2.505V         | 4.08V       |
| Joystick Y  | 80      | 160        | 240     | 1.165V      | 2.472V         | 3.857V      |
| Slider X    | 7       | 159        | 255     | 0.128V      | 2.505V         | 4.967V      |
| Slider Y    | ~0      | 181        | ~255    | -           | 2.85V          | -           |

## Voltage to ADC Formulas

**General Formula:**
```
ADC = (V - V_min) / (V_max - V_min) × (ADC_max - ADC_min) + ADC_min
```

**Specific Formulas:**
- Joystick X: `ADC ≈ 60.4 × V - 56.4`
- Joystick Y: `ADC ≈ 59.4 × V + 10.8`
- Slider X:   `ADC ≈ 51.2 × V + 0.5`

## Basic Usage

### 1. Include the Driver
```c
#include "driver/driver.h"
```

### 2. Initialize (call once in setup)
```c
driver_init();
```

### 3. Read Positions

**Joystick** (returns -1.0 to +1.0):
```c
Vec2 joy = driver_read_joystick();
// joy.x: -1.0 (left) to +1.0 (right)
// joy.y: -1.0 (down) to +1.0 (up)
```

**Slider** (returns 0.0 to 1.0):
```c
Vec2 slider = driver_read_slider();
// slider.x: 0.0 (left) to 1.0 (right)
// slider.y: 0.0 (bottom) to 1.0 (top)
```

**Raw ADC Values**:
```c
uint8_t jx, jy, sx, sy;
driver_read_raw_adc(&jx, &jy, &sx, &sy);
```

## Display Example

```c
// On OLED
char buffer[32];
Vec2 joy = driver_read_joystick();
snprintf(buffer, 32, "X:%+.2f Y:%+.2f", joy.x, joy.y);
oled_print_string(buffer, 0, 2);

// Via UART
printf("Joystick: X=%.2f Y=%.2f\n", joy.x, joy.y);
```

## Advanced Usage

### Get Direct Access to Structures
```c
AdcJoystick* joystick = driver_get_joystick();
Slider* slider = driver_get_slider();
```

### Custom Scaling
```c
// Scale joystick to 0-100 range
Vec2 scaled = joystick_get_position_scaled(
    &joystick->joystick, 
    0.0, 100.0,  // X range
    0.0, 100.0   // Y range
);
```

### Calibrate Joystick
```c
AdcJoystick* joy = driver_get_joystick();
// Keep joystick centered for 2 seconds
joystick_adc_calibrate_blocking(joy, 2000, 1.1);
```

## Example Files

- **src/main_driver_example.c** - Complete example with OLED
- **src/test/driver/** - Test code
- **DRIVER_README.md** - Full documentation
- **IMPLEMENTATION_SUMMARY.md** - Technical details

## Troubleshooting

**Values seem inverted?**
- Check if ADC min/max values are swapped
- Verify channel assignments match hardware

**Center position not at 0?**
- Run calibration: `joystick_adc_calibrate_blocking()`
- Adjust deadzone in `driver_init()`

**Values not matching OLED?**
- Compare raw ADC values with processed values
- Verify min/max calibration constants
- Check scaling formulas in driver.h

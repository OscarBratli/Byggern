# Joystick and Slider Driver

This driver provides a software interface to read joystick and slider positions from the ADC (MAX156).

## Hardware Configuration

The driver reads from 4 ADC channels:

- **ADC Channel 0**: Joystick Y axis
- **ADC Channel 1**: Joystick X axis
- **ADC Channel 2**: Slider/Touchpad X axis
- **ADC Channel 3**: Slider/Touchpad Y axis

## Measured Calibration Values

Based on hardware measurements, the following ADC values were recorded:

### Joystick Y (Channel 0)
- **Center**: ADC = 160, Voltage = 2.472V
- **Maximum**: ADC = 240, Voltage = 3.857V
- **Minimum**: ADC = 80, Voltage = 1.165V

### Joystick X (Channel 1)
- **Center**: ADC = 160 ± 1, Voltage = 2.505V
- **Maximum**: ADC = 251, Voltage = 4.08V
- **Minimum**: ADC = 61, Voltage = 0.934V

### Slider X (Channel 2)
- **Center**: ADC = 159, Voltage = 2.505V
- **Maximum**: ADC = 255, Voltage = 4.967V
- **Minimum**: ADC = 7, Voltage = 0.128V

### Slider Y (Channel 3)
- **Center**: ADC = 181, Voltage = 2.85V
- **Maximum**: Estimated at 255 (not provided in measurements)
- **Minimum**: Estimated at 0 (not provided in measurements)

## Voltage to ADC Relationship

The relationship between voltage and ADC value is approximately **linear**. The general formula is:

```
ADC = (Voltage - V_min) / (V_max - V_min) * (ADC_max - ADC_min) + ADC_min
```

Or equivalently:

```
Voltage = (ADC - ADC_min) / (ADC_max - ADC_min) * (V_max - V_min) + V_min
```

### Specific Relationships

**Joystick X:**
```
ADC = (V - 0.934) / (4.08 - 0.934) * (251 - 61) + 61
ADC ≈ (V - 0.934) / 3.146 * 190 + 61
ADC ≈ 60.4 * V - 56.4
```

**Joystick Y:**
```
ADC = (V - 1.165) / (3.857 - 1.165) * (240 - 80) + 80
ADC ≈ (V - 1.165) / 2.692 * 160 + 80
ADC ≈ 59.4 * V + 10.8
```

**Slider X:**
```
ADC = (V - 0.128) / (4.967 - 0.128) * (255 - 7) + 7
ADC ≈ (V - 0.128) / 4.839 * 248 + 7
ADC ≈ 51.2 * V + 0.5
```

## Usage

### Initialization

```c
#include "driver/driver.h"

// Initialize the driver (also initializes ADC)
driver_init();
```

### Reading Joystick Position

The joystick position is returned in the range **[-1.0, 1.0]** for both X and Y axes, with center at (0, 0).

```c
Vec2 joystick_pos = driver_read_joystick();
printf("Joystick: X = %.2f, Y = %.2f\n", joystick_pos.x, joystick_pos.y);
```

### Reading Slider Position

The slider position is returned in the range **[0.0, 1.0]** for both X and Y axes.

```c
Vec2 slider_pos = driver_read_slider();
printf("Slider: X = %.2f, Y = %.2f\n", slider_pos.x, slider_pos.y);
```

### Reading Raw ADC Values

You can also read the raw ADC values directly:

```c
uint8_t joy_x_adc, joy_y_adc, slider_x_adc, slider_y_adc;
driver_read_raw_adc(&joy_x_adc, &joy_y_adc, &slider_x_adc, &slider_y_adc);
printf("Joystick ADC: X = %d, Y = %d\n", joy_x_adc, joy_y_adc);
printf("Slider ADC: X = %d, Y = %d\n", slider_x_adc, slider_y_adc);
```

### Advanced Usage

You can access the underlying joystick and slider structures for more control:

```c
AdcJoystick* joystick = driver_get_joystick();
Slider* slider = driver_get_slider();

// Update and get scaled position
joystick_adc_update(joystick);
Vec2 scaled_pos = joystick_get_position_scaled(&joystick->joystick, 0.0, 100.0, 0.0, 100.0);

// Calibrate joystick (blocks for 2 seconds)
joystick_adc_calibrate_blocking(joystick, 2000, 1.1);
```

## Example Application

See `src/main_driver_example.c` for a complete example that:
1. Initializes the driver
2. Reads joystick and slider positions
3. Displays values on the OLED screen
4. Outputs values via UART

## Consistency with OLED Display

The driver is designed to return values that are consistent with the OLED display:

- Joystick values are centered around 0 with a configurable deadzone
- Slider values start at 0 and go to 1.0
- The scaling is based on the actual measured min/max ADC values from the hardware
- Raw ADC values can be displayed alongside processed values for verification

## API Reference

### Driver Functions

- `void driver_init(void)` - Initialize the driver and ADC
- `Vec2 driver_read_joystick(void)` - Read joystick position [-1.0, 1.0]
- `Vec2 driver_read_slider(void)` - Read slider position [0.0, 1.0]
- `void driver_read_raw_adc(...)` - Read raw ADC values
- `AdcJoystick* driver_get_joystick(void)` - Get joystick structure
- `Slider* driver_get_slider(void)` - Get slider structure

### Joystick Functions

- `AdcJoystick joystick_adc_create(...)` - Create ADC joystick
- `void joystick_adc_update(AdcJoystick *j)` - Update joystick from ADC
- `void joystick_adc_calibrate_blocking(...)` - Calibrate joystick
- `Vec2 joystick_get_position_centered(Joystick *j)` - Get position [-1, 1]
- `Vec2 joystick_get_position_scaled(...)` - Get position with custom scaling

### Slider Functions

- `Slider slider_create(...)` - Create slider
- `void slider_update(Slider *s, uint8_t adc_x, uint8_t adc_y)` - Update slider
- `Vec2 slider_get_position(Slider *s)` - Get position [0, 1]
- `Vec2 slider_get_position_scaled(...)` - Get position with custom scaling

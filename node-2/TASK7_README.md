# Task 7 - Servo Control and IR Goal Detection

## Overview
Task 7 implements PWM-controlled servo for the shooting mechanism and IR beam-break sensor for goal detection.

---

## Implemented Drivers

### 1. PWM Driver (`pwm.c/h`)
**Purpose:** Generate hardware PWM signals using SAM3X PWM Controller peripheral.

**Key Features:**
- Uses PWM Channel 1 (PWMH1) on pin PB13 (Arduino Due pin D21 = SIGNAL)
- 50Hz frequency (20ms period) for servo control
- Clock configuration: 84MHz MCK → /64 → 1.3125MHz CLKA
- Safe pulse width range: 900-2100µs (0.9-2.1ms)
- Hardware clamping to prevent servo damage

**API:**
- `pwm_init()` - Initialize PWM peripheral and configure pin
- `pwm_set_pulse_width_us(us)` - Set pulse width in microseconds
- `pwm_get_pulse_width_us()` - Read current pulse width

---

### 2. Servo Driver (`servo.c/h`)
**Purpose:** High-level servo position control with percentage-based interface.

**Key Features:**
- Position control: 0-100% maps to 900-2100µs
- Linear interpolation for smooth positioning
- Safety clamping at multiple layers
- Built on top of PWM driver

**API:**
- `servo_init()` - Initialize servo (starts at 50% center position)
- `servo_set_position(percent)` - Set servo angle (0-100%)
- `servo_get_position()` - Read current position percentage

---

### 3. IR Sensor Driver (`ir_sensor.c/h`)
**Purpose:** Detect ball passing through goal using IR beam-break sensor.

**Key Features:**
- Uses SAM3X internal ADC (12-bit, 0-4095 resolution)
- ADC Channel 0 on PA2 (Arduino Due pin A7)
- Auto-calibration for baseline detection
- Voltage range: 0-3.3V (3300mV)
- Persistent score tracking with debouncing

**Hardware:**
- IR LED transmitter: +5V → IR LED → 47Ω → GND
- IR photodiode: +5V → Photodiode → 330kΩ → GND (signal tapped before resistor)
- MCP602 op-amp as unity-gain buffer for noise immunity
- Signal output connects to A7 after voltage verification

**API:**
- `ir_sensor_init()` - Initialize ADC and configure PA2
- `ir_sensor_read_raw()` - Read 12-bit ADC value (0-4095)
- `ir_sensor_read_voltage_mv()` - Read voltage in millivolts (0-3300)
- `ir_sensor_is_beam_broken(threshold)` - Check if beam is broken
- `ir_sensor_calibrate()` - Measure baseline and suggest threshold
- `ir_sensor_get_score()` - Get current goal count
- `ir_sensor_reset_score()` - Reset score to 0
- `ir_sensor_increment_score()` - Add 1 to score

---

## Tests Implemented

### Test 1: PWM Range Verification (`task7_pwm_range_test()`)
**Purpose:** Verify PWM signal timing with oscilloscope.

**What it does:**
- Cycles through 5 pulse widths: 900, 1200, 1500, 1800, 2100µs
- Holds each for 3 seconds
- Displays current pulse width on serial output

**How to test:**
1. Edit `main.c`: uncomment `task7_pwm_range_test();`
2. Build and flash: `make && make flash`
3. Connect oscilloscope to Pin 21 (SIGNAL)
4. Verify pulse widths match expected values (0.9-2.1ms)
5. Check frequency is 50Hz (20ms period)

**Expected result:** Clean 50Hz PWM signal with correct pulse widths.

---

### Test 2: Joystick Servo Control (`task7_joystick_servo_control()`)
**Purpose:** Control servo angle with joystick Y-axis via CAN bus.

**What it does:**
- Receives joystick data from Node 1 over CAN (ID=0x00)
- Maps joystick Y-axis (0-100%) to servo position (inverted)
- Y=0 (top) → 100% → 2.1ms (max angle)
- Y=100 (bottom) → 0% → 0.9ms (min angle)
- 1% jitter filtering for smooth control
- No delays for instant response (~20ms update rate from Node 1)

**How to test:**
1. Ensure Node 1 is running and sending joystick data on CAN
2. Edit `main.c`: uncomment `task7_joystick_servo_control();`
3. Build and flash: `make && make flash`
4. Move joystick Y-axis up/down
5. Servo should follow joystick smoothly and instantly

**Expected result:** Servo responds immediately to joystick movements with inverted Y-axis control.

---

### Test 3: IR Sensor Test (`task7_ir_sensor_test()`)
**Purpose:** Test IR beam-break detection and goal scoring.

**What it does:**
- Initializes IR sensor ADC on PA2 (A7)
- Auto-calibrates baseline voltage with beam intact
- Sets detection threshold to 70% of baseline
- Continuously monitors beam status with 5-sample debouncing
- Detects goals on rising edge (broken → intact transition)
- Tracks and displays score
- Prints ADC value, voltage, status, debounce count, and score every ~100ms

**Hardware requirements:**
- IR LED circuit: +5V → IR LED → 47Ω → GND
- Photodiode circuit: +5V → Photodiode → 330kΩ → GND
- MCP602 op-amp configured as unity-gain buffer
- Op-amp output connected to A7 (PA2) on Arduino Due
- **CRITICAL:** Verify voltage with multimeter before connecting (must be 0-3.3V!)

**How to test:**
1. Build IR circuit with voltage verification (see IR_CONNECTIONS.md)
2. Measure output voltage: should be ~1.8V intact, ~0V blocked
3. Connect op-amp output to A7 only after voltage check
4. Edit `main.c`: uncomment `task7_ir_sensor_test();`
5. Build and flash: `make && make flash`
6. Monitor serial output at 9600 baud
7. Observe calibration baseline (~1800mV)
8. Block IR beam with hand/card → Status shows "BROKEN"
9. Remove blockage → "GOAL DETECTED!" message, score increments

**Expected result:**
- Stable voltage readings (~1800mV when intact)
- Clean transitions between INTACT/BROKEN states (debouncing prevents jitter)
- Accurate goal detection only on beam restoration
- No duplicate goals from noise or partial breaks

**Serial output example:**
```
Calibration complete: Baseline = 1776 mV
Suggested threshold: 1243 mV (70% of baseline)

ADC: 2134 (0x856) | Voltage: 1776 mV | Status: INTACT | Debounce: 5/5 | Score: 0
ADC:  142 (0x08E) | Voltage:  118 mV | Status: BROKEN | Debounce: 5/5 | Score: 0
ADC: 2128 (0x850) | Voltage: 1770 mV | Status: INTACT | Debounce: 5/5 | Score: 0

*** GOAL #1 DETECTED! ***

ADC: 2132 (0x854) | Voltage: 1774 mV | Status: INTACT | Debounce: 5/5 | Score: 1
```

---

## Switching Between Tests

Edit `/src/node-2/main.c` to choose which test runs:

```c
int main()
{
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS;
    uart_init(84000000, 9600);
    
    printf("\n\n=== Node 2 Starting ===\n");

    // Uncomment the test you want to run:
    
    // task7_pwm_range_test();           // Oscilloscope verification
    // task7_joystick_servo_control();   // Joystick servo control
    task7_ir_sensor_test();              // IR sensor testing (currently active)
    
    while (1) { }
}
```

Then rebuild and flash:
```bash
cd /home/student/Desktop/G46/Byggern/src/node-2
make && make flash
```

---

## Pin Connections Summary

| Function | Arduino Pin | SAM3X Pin | Connection |
|----------|-------------|-----------|------------|
| Servo PWM | D21 | PB13 (PWMH1) | → SIGNAL on motor shield |
| IR Sensor | A7 | PA2 (AD0) | ← From op-amp output |
| CAN RX | CANRX | PA1 (CANRX0) | ↔ CAN bus to Node 1 |
| CAN TX | CANTX | PA0 (CANTX0) | ↔ CAN bus to Node 1 |
| Serial Debug | USB | UART | Serial monitor 9600 baud |

---

## Common Issues & Solutions

### Servo Issues:
- **Servo jittery:** Check power supply, add capacitor across servo power
- **Wrong direction:** Joystick Y-axis is inverted (Y=0→max, Y=100→min)
- **No movement:** Verify Pin 21 connection to SIGNAL, check PWM with scope

### IR Sensor Issues:
- **Voltage too high (>3.3V):** Use smaller resistor or voltage divider - DO NOT CONNECT!
- **No signal change:** Check IR LED with phone camera, verify photodiode polarity
- **False goals:** Increase DEBOUNCE_COUNT in task7.c (currently 5)
- **Noisy readings:** Add 0.1µF capacitor across photodiode + resistor

### CAN Communication Issues:
- **No joystick data:** Check Node 1 is running and CAN_BR = 0x00290165
- **Wrong message ID:** Node 1 must send on ID=0x00
- **Slow response:** Node 1 should send at ~50Hz (20ms intervals)

---
## Documentation Files
- `TASK7_README.md` - This file (overview and testing guide)
---

## Next Steps

Once all three tests are working:
1. Integrate joystick servo control with IR goal detection
2. Send goal events from Node 2 to Node 1 via CAN
3. Display score on Node 1 OLED screen
4. Add game timer and win conditions
5. Create complete ping-pong game loop

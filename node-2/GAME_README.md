# Ping Pong Game System

## Overview
Complete game system with menu, calibration, gameplay, and high score tracking.

## Game Flow

### 1. Main Menu
- **Navigate**: Use joystick Y-axis (up/down)
- **Select**: Press joystick center button
- **Options**:
  - Start Game
  - High Scores

### 2. Motor Calibration (Automatic)
When you start a game, the system automatically calibrates the motor:
1. Manually center the motor
2. Press joystick button to start
3. Motor moves left (3 seconds)
4. Motor moves right (6 seconds)
5. Motor returns to center
6. Game starts automatically

### 3. Gameplay
- **Paddle Control**: Joystick X-axis (left/right)
- **Paddle Angle**: Joystick Y-axis (up/down)
- **Fire Solenoid**: Press joystick center button
- **Scoring**: Score increases when IR beam is broken (goal detected)
- **Game Over**: Triggered after a goal is scored

### 4. Game Over Screen
- Shows your final score
- Automatically adds score to high scores if it qualifies
- **Return to Menu**: Press joystick button (or wait 5 seconds)

### 5. High Scores
- Displays top 5 scores
- **Return to Menu**: Press joystick button

## Hardware Requirements

### Node 2 (Arduino Due)
- **Motor**: Controlled via PWM (DIR/EN pins)
- **Encoder**: Quadrature encoder for position feedback
- **Servo**: Paddle angle control
- **Solenoid**: Ball launcher (PB25)
- **IR Sensor**: Goal detection (ADC channel 0, PA2/A7)

### Node 1 (ATmega162)
- **Joystick**: X/Y axes + center button
- **CAN Bus**: Sends joystick data to Node 2

## CAN Message Format
**ID 0x00** (Node 1 → Node 2):
- `byte[0]`: Joystick X (0-100%)
- `byte[1]`: Joystick Y (0-100%)
- `byte[2]`: Joystick button (0=released, 1=pressed)
- `byte[3]`: Slider X (0-255)
- `byte[4]`: Slider Y (0-255)

## Key Features

### Motor Control
- PI position controller (Kp=0.08, Ki=0.001)
- Automatic calibration to find min/max range
- Smooth joystick-to-position mapping

### Goal Detection
- IR beam break sensor
- Automatic calibration at startup
- Threshold set to 60% of baseline

### Score Tracking
- Current score displayed on UART
- Top 5 high scores saved in memory
- Automatic high score list management

### Safety Features
- Motor stops on game over
- Solenoid 50ms pulse with rising edge detection
- Watchdog timer disabled for uninterrupted gameplay

## Debug Output
All game events are printed to UART (9600 baud):
- Menu selections
- Calibration progress
- Goal detection
- Score updates
- State transitions

## File Structure
```
src/node-2/
├── game.h          # Game system API
├── game.c          # Game implementation
├── main.c          # Main entry point (calls game_init/game_loop)
├── motor.c/h       # Motor control + PI controller
├── encoder.c/h     # Encoder reading
├── servo.c/h       # Servo control
├── solenoid.c/h    # Solenoid control
├── ir_sensor.c/h   # IR goal detection
├── can.c/h         # CAN communication
└── time.c/h        # Timing utilities
```

## Compilation
```bash
cd src/node-2
make           # Compile
make flash     # Flash to Arduino Due
```

## Usage Tips
1. **First Time Setup**: Make sure to center the motor manually during calibration
2. **IR Sensor**: Ensure the IR LED and photodiode are properly aligned
3. **Joystick**: Test joystick response before starting game
4. **CAN Bus**: Verify Node 1 is sending data (check UART output)

## Troubleshooting

### Motor doesn't move
- Check encoder connections
- Verify motor power supply
- Check DIR/EN pin connections

### Solenoid doesn't fire
- Check PB25 connection
- Verify relay and transistor circuit
- Check 12V power supply

### No goal detection
- Check IR sensor ADC reading (`ir_sensor_read_voltage_mv()`)
- Adjust threshold if needed
- Verify IR LED is powered

### Menu doesn't respond
- Check CAN communication
- Verify joystick button is working
- Check UART for debug messages

## Future Enhancements
- OLED display support for menu/scores on I/O board
- Persistent high scores (EEPROM/flash storage)
- Timer/countdown mode
- Multi-player support
- Sound effects via buzzer
- Difficulty levels (motor speed, servo sensitivity)

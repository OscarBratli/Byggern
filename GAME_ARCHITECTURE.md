# Clean Game Architecture

## Overview
**Node 1 (ATmega162)**: OLED menu with X-axis navigation, always sends CAN data  
**Node 2 (Arduino Due)**: Menu state (ignores joystick) → Game state (motor control + goal detection) → back to menu

## Node 1 - OLED Menu System

### Features
- **Navigation**: Joystick X-axis (Left=Start Game, Right=High Scores)
- **Button**: Select option / Toggle high scores view
- **Display**: Simple text menu on OLED
- **High Scores**: Stored locally in Node 1, displayed on OLED

### Files
- `src/test/game_menu/game_menu.h` - API
- `src/test/game_menu/game_menu.c` - Implementation

### CAN Communication
Always sends CAN message ID 0x00:
```
[joy_x (0-100), joy_y (0-100), joy_btn (0/1), 0, 0]
```

## Node 2 - Game Control

### State Machine
1. **MENU State**: 
   - Ignores joystick data
   - Waits for button press to start game
   - Motor stopped, servo centered

2. **PLAYING State**:
   - Auto-calibrates motor (11 seconds)
   - PI controller for position control
   - Joystick X → Motor position target
   - Joystick Y → Servo position
   - Button → Solenoid fire
   - IR sensor → Goal detection (one beam break = game over)

### Files
- `src/node-2/game.h` - API
- `src/node-2/game.c` - Implementation

### Initialization
```c
game_init();  // Initialize all peripherals + CAN
game_loop();  // Main state machine loop
```

### Game Flow
```
MENU → [Button Press] → PLAYING (calibrate) → [Goal Detected] → MENU
```

## Building

### Node 1 (ATmega162)
```bash
cd /home/student/Desktop/Byggern_G46/Byggern
make
make flash
```

### Node 2 (Arduino Due)
```bash
cd /home/student/Desktop/Byggern_G46/Byggern/src/node-2
make
make flash
```

## Key Design Decisions

✅ **Simple**: Minimal code, reuses existing motor control logic  
✅ **Clean separation**: Node 1 = display, Node 2 = game logic  
✅ **No communication needed**: Node 2 doesn't tell Node 1 about state changes  
✅ **One beam break = game over**: Immediate return to menu  
✅ **High scores on OLED**: No UART printf needed for scores  
✅ **X-axis navigation**: Easier to use than Y-axis  

## Next Steps (Optional)
- Implement high score saving/loading
- Add goal counter display on Node 1 during gameplay
- Add sound effects
- Add difficulty levels

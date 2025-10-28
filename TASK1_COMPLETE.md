# Task 1 Complete! ✅

## What We Did
Created a simple GPIO toggle program for Node 2 (ATSAM3X8E) that:
- Toggles pin PC23 (servo header signal) every 500ms
- Verifies the board is working before doing complex tasks
- Acts as a "Hello World" for embedded systems

## Files Modified
- `src/node-2/main.c` - Added GPIO toggle code

## How to Test

### Build the project:
```bash
cd /home/student/Desktop/G46/Byggern/src/node-2
make clean
make
```

### Flash to board:
```bash
make flash
```

### Verify with oscilloscope:
1. Connect scope probe to servo header signal pin (PC23/PWM0)
2. Should see square wave: 1Hz, 0-3.3V, 50% duty cycle

## Code Explanation
```c
// Enable clock for PIOC peripheral
PMC->PMC_PCER0 |= (1 << ID_PIOC);

// Configure PC23 as output
PIOC->PIO_PER |= PIO_PC23;   // PIO control (not peripheral function)
PIOC->PIO_OER |= PIO_PC23;   // Output enable
PIOC->PIO_PUDR |= PIO_PC23;  // Disable pull-up

// Toggle forever
while (1) {
    PIOC->PIO_SODR = PIO_PC23;  // Set Output Data Register (HIGH)
    delay_ms(500);
    PIOC->PIO_CODR = PIO_PC23;  // Clear Output Data Register (LOW)
    delay_ms(500);
}
```

## Success Criteria
✅ Code compiles without errors
✅ Binary flashes to board
✅ Oscilloscope shows 1Hz square wave on PC23
✅ Board doesn't reset/hang

## Next Task
Once verified, proceed to **Task 2: UART Communication on Node 2**

---

## Troubleshooting

### "openocd not found"
```bash
sudo apt install openocd
# Or install from source (see main README.md)
```

### "arm-none-eabi-gcc not found"
```bash
sudo apt install gcc-arm-none-eabi
```

### Flash fails / Board not detected
1. Check JTAG cable is connected properly
2. Ensure board has power
3. Try running openocd manually to see errors:
   ```bash
   openocd -f sam/openocd.cfg
   ```

### No signal on oscilloscope
1. Verify correct pin (PC23 on the servo header)
2. Check ground connection
3. Ensure board is actually running (not stuck in bootloader)
4. Try measuring with multimeter for steady 1.65V (average of square wave)

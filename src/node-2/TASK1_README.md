# Task 1: GPIO Toggle "Hello World" for Node 2

## What This Does
This program toggles GPIO pin PC23 (PIOC bit 23) high and low every 500ms. This pin is connected to the PWM0 output which goes to the servo header signal pin on the Node 2 shield.

## Hardware Setup
1. Connect your ATSAM3X8E board (Node 2)
2. Connect the JTAG debugger
3. Power the board

## Testing with Oscilloscope
1. **Connect oscilloscope probe to:**
   - Signal: Servo header pin (PWM0/PC23)
   - Ground: Any GND pin on the board

2. **Expected signal:**
   - Square wave
   - Frequency: ~1 Hz (500ms HIGH, 500ms LOW)
   - Voltage: 0V to 3.3V (or 5V depending on your board configuration)

## Flashing the Code

### Method 1: Using OpenOCD (recommended)
```bash
cd /home/student/Desktop/G46/Byggern/src/node-2
make flash
```

### Method 2: Manual OpenOCD
```bash
cd /home/student/Desktop/G46/Byggern/src/node-2

# Start OpenOCD in background
openocd -f sam/openocd.cfg &

# In another terminal, use GDB
arm-none-eabi-gdb build/main.elf
(gdb) target remote :3333
(gdb) monitor reset halt
(gdb) load
(gdb) continue
```

## Verification
✅ **Success if you see:**
- Oscilloscope shows square wave at ~1 Hz
- LED on board may also blink if connected to same pin
- Board is running (not stuck in reset)

❌ **Troubleshooting:**
- **No signal**: Check connections, verify pin PC23 is accessible
- **Won't flash**: Check JTAG connections, ensure OpenOCD is configured correctly
- **Compile errors**: Make sure you're in the correct directory and have arm-none-eabi-gcc installed

## What's Happening in the Code

```c
// Enable PIOC peripheral clock
PMC->PMC_PCER0 |= (1 << ID_PIOC);

// Configure PC23 as GPIO output
PIOC->PIO_PER |= PIO_PC23;   // Enable PIO control
PIOC->PIO_OER |= PIO_PC23;   // Output enable
PIOC->PIO_PUDR |= PIO_PC23;  // Disable pull-up

// Toggle loop
while (1) {
    PIOC->PIO_SODR = PIO_PC23;  // Set high
    delay_ms(500);
    PIOC->PIO_CODR = PIO_PC23;  // Set low
    delay_ms(500);
}
```

## Next Steps
Once this works, you've verified:
- ✅ Board is powered correctly
- ✅ JTAG programming works
- ✅ SAM3X8E is executing code
- ✅ GPIO peripherals are functional

Now you can move on to **Task 2: UART Communication**

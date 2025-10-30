1. The timer/counter module of the ATsam is a versatile module and can generate timer inter-
rupts, PWM signals and more. Create a driver for the timer/counter module which allows
you to use the PWM functionality. If you also implement the timer interrupt it might save
you time when implementing the controller since most of you will most likely want to use
timer interrupt there. You shall use the PWM module for the arduino due instead.

2. Create a PWM and/or servo driver which will use your controller output as an input and
calculate the correct duty cycle/on-time which you will provide to your timer/counter driver.
Also implement safety features which will never let the PWM go out of the servo’s valid
range.

3. Use an oscilloscope to verify that your driver in fact never goes outside the valid range of the
servo (0.9-2.1ms).

4. Connect the servo on the game board to the PWM output on top of the Arduino shield.

5. Use the joystick position sent from Node 1 to control the servo position.

6. “Goals” are registered by blocking an IR beam. Install the IR-LED and IR-photodiode in
the two holes located at the side walls of the game board.

7. Connect the IR diodes in a way that makes it possible to detect when the IR beam is blocked.
An example is given in Figure 25. You might consider implementing an analog filter. Check
that the signal is correct before connecting it to the microcontroller

8. As the IR signal is noisy and unstable you need to use the internal ADC of the Arduino to
read the signal and filter out valid signal states.

9. Create a driver that will read the IR signal. You may want to implement a digital filter to
reduce noise.

10. Create a function that is able to count the score. This will later be used for the game
applicatio
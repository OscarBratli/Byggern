1.  Connect the encoder input to the motor encoder by using the designated cable. Use TC2
of the ATSAM3X8E in quadrature decoder mode and verify that it reads and decodes the
motor motion.
2. Connect the shield’s 4mm jacks to a 12V power supply, and the shield’s 2mm jacks to the
motor.
3. Develop code so that the motor speed and direction can be set by joystick position (x-axis
for motor, y-axis for servo) using the the A3959 motor driver in phase/enable mode. You
should now be able to play the game, although it is hard to control.
4. Create a position based controller (closed-loop, using feedback from the encoder) for the
motor so that the position is easier to control. A PI controller should be sufficient. Exact
tuning is not important, but the game must be playable with the controller.
5. Being a course/lab provided by the cybernetics department, a closed-loop/feedback position
based controller is the minimum requirement for approval of the exercise and the
lab exam. If you prefer a speed based controller, a position based controller must still be
implemented.
6. Extend the code to include solenoid triggering when one of the joystick buttons is pushed.
It should generate a pulse just long enough to hit the ball.
1. Recommended but optional: Write a program which enables (i.e. sets high) the servo header
signal pin, and verify your result with an oscilloscope. Think of it as a ”hello world” for the
Node 2 shield.
2. It is useful to implement serial communication also on Node 2. A simple library is available
in Blackboard. Make sure to find/set the baudrate in uart.c.
3. Connect the CAN transceiver MCP2551 on Node 1 using the information provided in the
datasheet. You can use the 22k resistor for slew-rate limiting.
4. Import the CAN library for node 2 to your project. The library is available in Blackboard.
Feel free to adapt it to your needs as the project progresses, e.g. by implementing message
ID masks, circular buffers etc.
5. Connect Node 1 and Node 2 together using the CAN bus, conforming to the “AN228: A
CAN Physical Layer Discussion” document. Note: The CAN TERM jumper on the Node 2
shield must be connected at this point.
6. Decide the CAN bus bit-timing by writing to the CNFx registers on MCP2515. Make sure to
match the configuration in the CAN BR register on ATSAM3X8E.
7. Test the system again, but now with the CAN controller of node 1 in normal mode. Node 2
should be able to reuse the upper level code generated in the previous exercise.
8. Make a joystick driver that can send joystick position from Node 1 to Node 2 over the CAN
bus.
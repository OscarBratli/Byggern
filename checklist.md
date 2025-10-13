1. place the IO board connection adapter in a convenient location on your bread board, oriented
  such that it may be neatly connected to the MCU’s SPI pins.  (X)

2. Connect the SPI pins, and choose suitable pins for the slave select signals. (X)

3. Write an SPI driver for your ATmega. It should have the following capabilities:
• Select slave n  (X)
• Write byte      (X)
• Read byte       (X)
• Also convenient: Read/write n bytes  (X)
4. Write an OLED display interface on top of the SPI driver:
• Initialisation  (X)
• Go to line      (X)
• Go to column    (X)
• Printf (or at least your own simplified version) (X)
• Funny graphics?
• Remember that the SPI driver itself will also be used for CAN bus
5. You have access to several fonts for use with the display, but they are too large to be stored  (X)
in data memory. Store them in PROGMEM and use them from there.
6. Test your driver/interface (it’s scopin’ time!) (X)
7. Connect the IO board, and try to send data to the OLED display. (X)
8. Make a framework for a user interface that can be navigated in using the joystick. As a
minimum, it should be able to let the user navigate up and down in a menu consisting of a
list of strings, and return the menu position when the joystick button is clicked. Also, think
about how you would implement sub-menus using this framework.
9. Expand your interface to be able to receive data from the buttons on the IO board. You
could for instance toggle an LED on your breadboard
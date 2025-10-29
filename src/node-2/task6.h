/*
 * task6.h - Node 2 Test Functions Header
 * 
 * Header file for Node 2 test functions
 */

#ifndef TASK6_H
#define TASK6_H

#include <stdint.h>

// Simple delay function
void delay_ms(uint32_t ms);

// Task test functions
void task1_gpio_toggle_test(void);
void task2_uart_test(void);
void task3_can_test(void);
void task3_can_test_with_joystick_decoder(void); // CAN test with joystick decoder
void run_all_tests(void);

#endif /* TASK6_H */
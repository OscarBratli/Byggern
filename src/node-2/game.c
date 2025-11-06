/**
 * @file game.c
 * @brief Minimal Game System - Just calls task8_motor_calibration
 */
#include "test/task8.h"
#include "game.h"
#include "sam.h"
#include "can.h"
#include "motor.h"
#include "encoder.h"
#include "servo.h"
#include "ir_sensor.h"
#include "solenoid.h"
#include "time.h"
#include <stdio.h>

static game_state_t current_state = GAME_STATE_MENU;

void game_init(void) {
    motor_init();
    encoder_init();
    servo_init();
    solenoid_init();
    ir_sensor_init();
    
    // CAN init
    can_init((CanInit){.brp=20, .propag=2, .phase1=7, .phase2=6, .sjw=1, .smp=0}, 0);
    CAN0->CAN_BR = 0x00290165;
}

void game_loop(void) {
    CanMsg msg;
    static bool last_button_state = false;
    
    while (1) {
        switch (current_state) {
            case GAME_STATE_MENU:
                // Wait for start signal (joystick button with edge detection)
                if (can_rx(&msg) && msg.id == 0x00 && msg.length >= 3) {
                    bool button_pressed = (msg.byte[2] != 0);
                    
                    // Rising edge detection - button just pressed
                    if (button_pressed && !last_button_state) {
                        printf("Button detected! Starting game...\n");
                        current_state = GAME_STATE_PLAYING;
                        
                        // Call task8_motor_calibration - it handles everything
                        // Returns when beam is broken (game over)
                        task8_motor_calibration();
                        
                        // Return to menu after game ends
                        current_state = GAME_STATE_MENU;
                        motor_set_signed(0);
                        printf("Game ended, back to menu\n");
                        
                        // Reset button state so button presses during game don't trigger new game
                        last_button_state = false;
                    } else {
                        // Update button state when not starting game
                        last_button_state = button_pressed;
                    }
                }
                time_spinFor(msecs(50));
                break;
                
            case GAME_STATE_PLAYING:
                // Should never reach here since task8_motor_calibration loops internally
                current_state = GAME_STATE_MENU;
                motor_set_signed(0);
                break;
        }
    }
}

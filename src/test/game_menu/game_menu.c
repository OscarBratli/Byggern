/**
 * @file game_menu.c
 * @brief Simple OLED Menu with X-axis navigation
 */

#include "game_menu.h"
#include "../../oled/oled.h"
#include "../../joystick/joystick.h"
#include "../../mcp2515/mcp2515.h"
#include "../../can/can.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

typedef enum {
    MENU_START_GAME,
    MENU_HIGH_SCORES,
    MENU_COUNT
} menu_option_t;

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_HIGH_SCORES
} display_state_t;

static menu_option_t selected_option = MENU_START_GAME;
static display_state_t display_state = STATE_MENU;
static uint32_t high_scores[5] = {0, 0, 0, 0, 0};
static bool display_needs_update = true;

void game_menu_init(void) {
    oled_init();
    mcp2515_init();
    can_init_normal();
}

void game_menu_loop(void) {
    // Read joystick
    joystick_pos_t joy = joystick_get_position();
    
    // State-based behavior
    if (display_state == STATE_MENU) {
        // Navigate with X-axis (only in menu)
        menu_option_t old_option = selected_option;
        if (joy.x < 30) {
            selected_option = MENU_START_GAME;
        } else if (joy.x > 70) {
            selected_option = MENU_HIGH_SCORES;
        }
        
        // Check if display needs update due to selection change
        if (old_option != selected_option) {
            display_needs_update = true;
        }
        
        // Button press
        static bool last_button = false;
        if (joy.button && !last_button) {  // Rising edge detection
            if (selected_option == MENU_HIGH_SCORES) {
                display_state = STATE_HIGH_SCORES;
                display_needs_update = true;
            } else if (selected_option == MENU_START_GAME) {
                display_state = STATE_PLAYING;
                display_needs_update = true;
            }
        }
        last_button = joy.button;
        
    } else if (display_state == STATE_HIGH_SCORES) {
        // Button press to go back
        static bool last_button_hs = false;
        if (joy.button && !last_button_hs) {
            display_state = STATE_MENU;
            display_needs_update = true;
        }
        last_button_hs = joy.button;
        
    } else if (display_state == STATE_PLAYING) {
        // In playing state: DON'T consume button presses
        // Just pass joystick data through to Node 2 via CAN
        // Node 2 will handle the button for solenoid firing
        
        // Check for game over message from Node 2 (CAN ID 0x01)
        can_message_t game_over_msg;
        if (can_receive_message(&game_over_msg)) {
            if (game_over_msg.id == 0x01 && game_over_msg.data[0] == 0xFF) {
                // Game over received from Node 2
                display_state = STATE_MENU;
                display_needs_update = true;
            }
        }
        
        // Only exit on beam break (game over message from Node 2)
        // No manual escape with joystick position
    }
    
    // Only update display when needed
    if (display_needs_update) {
        oled_clear_screen();
        
        if (display_state == STATE_HIGH_SCORES) {
            oled_print_string("HIGH SCORES", 0, 0);
            
            for (int i = 0; i < 5; i++) {
                char buf[16];
                snprintf(buf, sizeof(buf), "%d. %lu", i+1, high_scores[i]);
                oled_print_string(buf, 0, (i+1)*8);
            }
            
            oled_print_string("BTN=Back", 0, 56);
            
        } else if (display_state == STATE_PLAYING) {
            oled_print_string("GAME PLAYING", 20, 10);
            oled_print_string("- - - - -", 30, 25);
            oled_print_string("Control with", 15, 35);
            oled_print_string("joystick!", 25, 45);
            
        } else {  // STATE_MENU
            if (selected_option == MENU_START_GAME) {
                oled_print_string("> START GAME", 10, 20);
                oled_print_string("  HIGH SCORES", 10, 35);
            } else {
                oled_print_string("  START GAME", 10, 20);
                oled_print_string("> HIGH SCORES", 10, 35);
            }
            
            oled_print_string("Move X, Press BTN", 0, 56);
        }
        
        display_needs_update = false;
    }
    
    // Send CAN data at controlled rate (20ms = 50Hz)
    can_message_t msg;
    msg.id = 0x00;
    msg.length = 5;
    msg.data[0] = joy.x;
    msg.data[1] = joy.y;
    msg.data[2] = joy.button;
    msg.data[3] = 0; // slider_left
    msg.data[4] = 0; // slider_right
    can_send_message(&msg);
    
    _delay_ms(20);  // 50Hz update rate
}

// Menu module - Complete implementation
#include "menu.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "ioboard/ioboard.h"
#include "adc/adc.h"
#include <util/delay.h>
#include <stdio.h>

// Menu system variables (moved from main.c)
typedef enum {
    MAIN_MENU,
    SUB_MENU
} menu_state_t;

menu_state_t current_menu = MAIN_MENU;   // Current menu level
uint8_t menu_selection = 0;              // Currently selected menu item
uint8_t submenu_selection = 0;           // Currently selected submenu item
uint8_t selected_main_item = 0;          // Which main menu item was selected
uint8_t last_menu_selection = 255;       // Previous menu selection for change detection
bool last_button_state = false;          // Last joystick button state
uint8_t navigation_counter = 0;          // Counter for navigation debouncing



void display_menu(void)
{
    oled_clear_screen();
    oled_print_string("Main Menu", 0, 0);
    
    // Display each menu item directly to save RAM
    for (uint8_t i = 0; i < 4; i++) {
        // Show cursor for selected item
        if (i == menu_selection) {
            oled_print_char('>', 0, i + 2);  // Cursor at start of line
        } else {
            oled_print_char(' ', 0, i + 2);  // Space for unselected items
        }
        
        // Display menu item text directly 
        switch(i) {
            case 0: oled_print_string("New Game", 8, i + 2); break;
            case 1: oled_print_string("High Score", 8, i + 2); break;
            case 2: oled_print_string("Calibrate", 8, i + 2); break;
            case 3: oled_print_string("Settings", 8, i + 2); break;
        }
    }
}

void display_submenu(void)
{
    oled_clear_screen();
    
    // Display title based on selected main menu item
    switch(selected_main_item) {
        case 0: oled_print_string("New Game", 0, 0); break;
        case 1: oled_print_string("High Score", 0, 0); break;
        case 2: oled_print_string("Calibrate", 0, 0); break;
        case 3: oled_print_string("Settings", 0, 0); break;
    }
    
    // Display submenu items based on which main menu was selected
    for (uint8_t i = 0; i < 4; i++) {
        // Show cursor for selected item
        if (i == submenu_selection) {
            oled_print_char('>', 0, i + 2);  // Cursor at start of line
        } else {
            oled_print_char(' ', 0, i + 2);  // Space for unselected items
        }
        
        // Display context-specific submenu items
        switch(selected_main_item) {
            case 0: // New Game submenu
                switch(i) {
                    case 0: oled_print_string("Easy", 8, i + 2); break;
                    case 1: oled_print_string("Medium", 8, i + 2); break;
                    case 2: oled_print_string("Hard", 8, i + 2); break;
                    case 3: oled_print_string("Back", 8, i + 2); break;
                }
                break;
            case 1: // High Score submenu  
                switch(i) {
                    case 0: oled_print_string("View", 8, i + 2); break;
                    case 1: oled_print_string("Reset", 8, i + 2); break;
                    case 2: oled_print_string("Upload", 8, i + 2); break;
                    case 3: oled_print_string("Back", 8, i + 2); break;
                }
                break;
            case 2: // Calibrate submenu
                switch(i) {
                    case 0: oled_print_string("Joystick", 8, i + 2); break;
                    case 1: oled_print_string("Slider", 8, i + 2); break;
                    case 2: oled_print_string("Test", 8, i + 2); break;
                    case 3: oled_print_string("Back", 8, i + 2); break;
                }
                break;
            case 3: // Settings submenu
                switch(i) {
                    case 0: oled_print_string("Difficulty", 8, i + 2); break;
                    case 1: oled_print_string("Debug", 8, i + 2); break;
                    case 2: oled_print_string("About", 8, i + 2); break;
                    case 3: oled_print_string("Back", 8, i + 2); break;
                }
                break;
        }
    }
}

void menu_selector(void)
{
       // Read joystick position and button from ADC/GPIO
    joystick_pos_t joy_pos = joystick_get_position();
    
    // Get current selection pointer based on menu state
    uint8_t* current_selection = (current_menu == MAIN_MENU) ? &menu_selection : &submenu_selection;
    
    // Handle up/down navigation with proper debouncing
    // Use larger dead zones and counter-based debouncing to prevent noise
    if (joy_pos.x > 90) {
        // Joystick moved up
        navigation_counter++;
        if (navigation_counter > 3) {  // Require multiple consecutive readings
            if (*current_selection > 0) {
                (*current_selection)--;
            }
            navigation_counter = 0;  // Reset counter
        }
    } else if (joy_pos.x < 25) {
        // Joystick moved down  
        navigation_counter++;
        if (navigation_counter > 3) {  // Require multiple consecutive readings
            if (*current_selection < 3) {
                (*current_selection)++;
            }
            navigation_counter = 0;  // Reset counter
        }
    } else {
        // Joystick in neutral position
        navigation_counter = 0;
    }
    
    // Handle selection using joystick center button (connected to PB1)
    if (joy_pos.button && !last_button_state) {
        // Button pressed (rising edge)
        
        if (current_menu == MAIN_MENU) {
            // Main menu selection - enter submenu
            selected_main_item = menu_selection;
            current_menu = SUB_MENU;
            submenu_selection = 0;  // Reset submenu selection
            printf("Entered submenu for item %d\r\n", selected_main_item);
            
        } else {
            // Submenu selection
            if (submenu_selection == 3) {
                // "Back" option selected - return to main menu
                current_menu = MAIN_MENU;
                printf("Returned to main menu\r\n");
            } else {
                // Handle specific submenu actions
                printf("Selected: Main=%d, Sub=%d\r\n", selected_main_item, submenu_selection);
                
                // Execute specific actions based on selection
                switch(selected_main_item) {
                    case 0: // New Game
                        oled_clear_screen();
                        switch(submenu_selection) {
                            case 0: oled_print_string("Starting Easy", 0, 2); break;
                            case 1: oled_print_string("Starting Med", 0, 2); break; 
                            case 2: oled_print_string("Starting Hard", 0, 2); break;
                        }
                        oled_print_string("Press joy btn", 0, 4);
                        // Wait for joystick button press
                        while (1) {
                            joystick_pos_t joy = joystick_get_position();
                            if (joy.button) break;
                            _delay_ms(50);
                        }
                        break;
                    case 1: // High Score
                        oled_clear_screen();
                        switch(submenu_selection) {
                            case 0: oled_print_string("Viewing Scores", 0, 2); break;
                            case 1: oled_print_string("Scores Reset", 0, 2); break;
                            case 2: oled_print_string("Uploading...", 0, 2); break;
                        }
                        oled_print_string("Press joy btn", 0, 4);
                        // Wait for joystick button press
                        while (1) {
                            joystick_pos_t joy = joystick_get_position();
                            if (joy.button) break;
                            _delay_ms(50);
                        }
                        break;
                    case 2: // Calibrate
                        switch(submenu_selection) {
                            case 0: // Calibrate Joystick
                                oled_clear_screen();
                                oled_print_string("Calibrating...", 0, 1);
                                oled_print_string("Move joystick", 0, 2);
                                oled_print_string("around fully", 0, 3);
                                oled_print_string("Press joy btn", 0, 4);
                                oled_print_string("when done", 0, 5);
                                
                                joystick_reset_calibration();
                                
                                // Calibration loop - continuously update calibration
                                bool calibrating = true;
                                while (calibrating) {
                                    // Read raw ADC without triggering calibration in joystick_get_position()
                                    uint16_t adc_x = adc_read(1);  // JOYSTICK_ADC_X_CHANNEL
                                    uint16_t adc_y = adc_read(0);  // JOYSTICK_ADC_Y_CHANNEL
                                    
                                    // Manually call calibration with raw values
                                    joystick_calibrate_now();
                                    
                                    // Check for button press to finish calibration
                                    joystick_pos_t joy = joystick_get_position();
                                    if (joy.button) {
                                        calibrating = false;
                                        
                                        // Wait for button release to avoid immediate menu selection
                                        while (joy.button) {
                                            joy = joystick_get_position();
                                            _delay_ms(50);
                                        }
                                    }
                                    _delay_ms(50);
                                }
                                
                                // Show completion message
                                oled_clear_screen();
                                oled_print_string("Calibration", 0, 1);
                                oled_print_string("Complete!", 0, 2);
                                oled_print_string("Press joy btn", 0, 4);
                                printf("Joystick calibrated!\r\n");
                                
                                // Wait for button press to continue
                                while (1) {
                                    joystick_pos_t joy = joystick_get_position();
                                    if (joy.button) break;
                                    _delay_ms(50);
                                }
                                break;
                            case 1: // Calibrate Slider  
                                oled_clear_screen();
                                oled_print_string("Slider calib", 0, 2);
                                oled_print_string("not impl yet", 0, 3);
                                oled_print_string("Press joy btn", 0, 4);
                                // Wait for joystick button press
                                while (1) {
                                    joystick_pos_t joy = joystick_get_position();
                                    if (joy.button) break;
                                    _delay_ms(50);
                                }
                                break;
                            case 2: // Test - Show live joystick display
                                // Enter test mode - continuous display until button pressed
                                oled_clear_screen();
                                oled_print_string("Test Mode", 0, 0);
                                oled_print_string("Press joy btn", 0, 1);
                                oled_print_string("to exit", 0, 2);
                                _delay_ms(2000);
                                
                                // Test loop - show joystick values until button pressed
                                while (1) {
                                    display_joystick();
                                    joystick_pos_t test_joy = joystick_get_position();
                                    if (test_joy.button) {
                                        break; // Exit test mode
                                    }
                                }
                                printf("Exited test mode\r\n");
                                break;
                        }
                        break;
                    case 3: // Settings
                        oled_clear_screen();
                        switch(submenu_selection) {
                            case 0: oled_print_string("Difficulty Set", 0, 2); break;
                            case 1: oled_print_string("Debug Mode", 0, 2); break;
                            case 2: oled_print_string("Game v1.0", 0, 2); break;
                        }
                        oled_print_string("Press joy btn", 0, 4);
                        // Wait for joystick button press
                        while (1) {
                            joystick_pos_t joy = joystick_get_position();
                            if (joy.button) break;
                            _delay_ms(50);
                        }
                        break;
                }
            }
        }
        
        // Force menu redraw
        last_menu_selection = 255;
    }
    
    // Only redraw menu if selection changed (reduces flickering)
    uint8_t current_sel = (current_menu == MAIN_MENU) ? menu_selection : submenu_selection;
    if (current_sel != last_menu_selection) {
        if (current_menu == MAIN_MENU) {
            display_menu();
        } else {
            display_submenu();
        }
        last_menu_selection = current_sel;
    }
    
    // Update button state for debouncing
    last_button_state = joy_pos.button;
    
    _delay_ms(50); // Small delay for responsiveness
}
// Empty stub functions to avoid linker errors
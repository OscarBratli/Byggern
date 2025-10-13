#include <util/delay.h>
#include <stdio.h>
#include <stdbool.h>
#include "spi/spi.h"
#include "ioboard/ioboard.h"
#include "uart/uart.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "adc/adc.h"

// Function declarations
void display_menu(void);
void display_submenu(void);

// Menu system variables
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

void setup(void)
{
    // Initialize hardware
    uart_init(MYUBRR);
    adc_init();
    joystick_init();
    spi_setup();
    oled_init();
    ioboard_init();
    
    // Initial display
    oled_clear_screen();
    display_menu();
 
}

void display_menu(void)
{
    oled_clear_screen();
    oled_print_string("PowerUP Menu!", 0, 0);
    
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
            case 0: oled_print_string("Hansa", 8, i + 2); break;
            case 1: oled_print_string("Mack", 8, i + 2); break;
            case 2: oled_print_string("Aass", 8, i + 2); break;
            case 3: oled_print_string("Frydelund", 8, i + 2); break;
        }
    }
}

void display_submenu(void)
{
    oled_clear_screen();
    
    // Display title - use switch to save RAM instead of array
    switch(selected_main_item) {
        case 0: oled_print_string("Hansa Sub", 0, 0); break;
        case 1: oled_print_string("Mack Sub", 0, 0); break;
        case 2: oled_print_string("Aass Sub", 0, 0); break;
        case 3: oled_print_string("Frydelund", 0, 0); break;
    }
    
    // Display submenu items directly to save RAM
    for (uint8_t i = 0; i < 4; i++) {
        // Show cursor for selected item
        if (i == submenu_selection) {
            oled_print_char('>', 0, i + 2);  // Cursor at start of line
        } else {
            oled_print_char(' ', 0, i + 2);  // Space for unselected items
        }
        
        // Display submenu item text directly
        switch(i) {
            case 0: oled_print_string("Info", 8, i + 2); break;
            case 1: oled_print_string("Settings", 8, i + 2); break;
            case 2: oled_print_string("Test", 8, i + 2); break;
            case 3: oled_print_string("Back", 8, i + 2); break;
        }
    }
}

void loop(void)
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
                // Other submenu options
                printf("Selected submenu item %d from main item %d\r\n", submenu_selection, selected_main_item);
                
                // Show feedback
                oled_print_string("AWESOME!", 64, submenu_selection + 2);
                _delay_ms(1000);
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

int main(void)
{
    setup();
    
    while (1) {
        loop();
    }
    return 0;
} 
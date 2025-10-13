#include "menu.h"
#include "oled/oled.h"
#include "joystick/joystick.h"
#include "ioboard/ioboard.h"
#include <string.h>
#include <util/delay.h>
#include <stdio.h>

// === Minimal Menu Actions ===
static bool led_state = false;
void menu_action_led_toggle(void) {
    led_state = !led_state;
    ioboard_led_set(0, led_state);
    oled_clear_screen();
    oled_print_string("LED", 0, 2);
    oled_print_string(led_state ? "ON" : "OFF", 0, 3);
    _delay_ms(500);
}

void menu_action_test_ioboard(void) {
    oled_clear_screen();
    oled_print_string("IO Test", 0, 0);
    
    // Simple one-shot test
    ioboard_buttons_t buttons = ioboard_read_buttons();
    oled_print_string("R:", 0, 2);
    oled_print_char(buttons.right + '0', 16, 2);
    oled_print_string("L:", 0, 3);
    oled_print_char(buttons.left + '0', 16, 3);
    
    _delay_ms(1500);
} 

// === Minimal Menu Setup ===
void menu_setup_example(void) {
    // Create main menu only
    menu_t* main_menu = menu_create("Main", NULL);
    
    // Add minimal items
    menu_add_action_item(main_menu, "IO Test", menu_action_test_ioboard);
    menu_add_action_item(main_menu, "LED", menu_action_led_toggle);
    
    // Activate main menu
    menu_set_active(main_menu);
}

// Global menu system state
static menu_system_t menu_system = {0};

// Input debouncing
static uint8_t last_joystick_y = 50;  // Center position
static bool button_pressed_last = false;
static uint32_t last_input_time = 0;
#define INPUT_DEBOUNCE_MS 200

// === Private Helper Functions ===

static void menu_clamp_selection(menu_t* menu) {
    if (menu->selected_item >= menu->item_count) {
        menu->selected_item = menu->item_count - 1;
    }
}

static void menu_update_scroll(menu_t* menu) {
    // Update scroll offset to keep selected item visible
    if (menu->selected_item < menu->scroll_offset) {
        menu->scroll_offset = menu->selected_item;
    } else if (menu->selected_item >= menu->scroll_offset + MENU_DISPLAY_LINES) {
        menu->scroll_offset = menu->selected_item - MENU_DISPLAY_LINES + 1;
    }
}

static void menu_draw_cursor(uint8_t line) {
    // Draw selection cursor (arrow)
    oled_print_char('>', 0, line + 1);  // +1 to skip title line
}

static void menu_draw_item(menu_t* menu, uint8_t item_index, uint8_t display_line) {
    uint8_t x_offset = 8;  // Leave space for cursor
    
    // Clear line first
    for (uint8_t x = 0; x < 128; x += 8) {
        oled_print_char(' ', x, display_line + 1);
    }
    
    // Draw item text
    oled_print_string(menu->items[item_index].text, x_offset, display_line + 1);
    
    // Draw item type indicator
    switch (menu->items[item_index].type) {
        case MENU_ITEM_SUBMENU:
            oled_print_char('>', 120, display_line + 1);  // Right arrow for submenu
            break;
        case MENU_ITEM_BACK:
            oled_print_char('<', 120, display_line + 1);  // Left arrow for back
            break;
        case MENU_ITEM_ACTION:
        default:
            // No special indicator for action items
            break;
    }
}

// === Public Menu Functions ===

void menu_init(void) {
    menu_system.current_menu = NULL;
    menu_system.root_menu = NULL;
    menu_system.menu_active = false;
    menu_system.needs_redraw = true;
}

menu_t* menu_create(const char* title, menu_t* parent) {
    static menu_t menus[2];  // Only 2 menus max to save RAM
    static uint8_t menu_count = 0;
    
    if (menu_count >= 2) return NULL;  // Out of menu slots
    
    menu_t* menu = &menus[menu_count++];
    
    // Initialize menu
    strncpy(menu->title, title, MAX_MENU_TEXT_LEN - 1);
    menu->title[MAX_MENU_TEXT_LEN - 1] = '\0';
    menu->item_count = 0;
    menu->selected_item = 0;
    menu->scroll_offset = 0;
    menu->parent = parent;
    
    // Clear all items
    for (uint8_t i = 0; i < MAX_MENU_ITEMS; i++) {
        menu->items[i].text[0] = '\0';
        menu->items[i].type = MENU_ITEM_ACTION;
        menu->items[i].action = NULL;
    }
    
    return menu;
}

void menu_add_action_item(menu_t* menu, const char* text, menu_action_func_t action) {
    if (!menu || menu->item_count >= MAX_MENU_ITEMS) return;
    
    menu_item_t* item = &menu->items[menu->item_count];
    strncpy(item->text, text, MAX_MENU_TEXT_LEN - 1);
    item->text[MAX_MENU_TEXT_LEN - 1] = '\0';
    item->type = MENU_ITEM_ACTION;
    item->action = action;
    
    menu->item_count++;
}

void menu_add_submenu_item(menu_t* menu, const char* text, menu_t* submenu) {
    if (!menu || menu->item_count >= MAX_MENU_ITEMS) return;
    
    menu_item_t* item = &menu->items[menu->item_count];
    strncpy(item->text, text, MAX_MENU_TEXT_LEN - 1);
    item->text[MAX_MENU_TEXT_LEN - 1] = '\0';
    item->type = MENU_ITEM_SUBMENU;
    item->submenu = submenu;
    
    menu->item_count++;
}

void menu_add_back_item(menu_t* menu) {
    if (!menu || menu->item_count >= MAX_MENU_ITEMS) return;
    
    menu_add_action_item(menu, "Back", NULL);
    menu->items[menu->item_count - 1].type = MENU_ITEM_BACK;
}

void menu_navigate(menu_navigation_t direction) {
    if (!menu_system.current_menu) return;
    
    menu_t* menu = menu_system.current_menu;
    
    switch (direction) {
        case MENU_NAV_UP:
            if (menu->selected_item > 0) {
                menu->selected_item--;
            } else {
                menu->selected_item = menu->item_count - 1;  // Wrap to bottom
            }
            menu_update_scroll(menu);
            menu_system.needs_redraw = true;
            break;
            
        case MENU_NAV_DOWN:
            if (menu->selected_item < menu->item_count - 1) {
                menu->selected_item++;
            } else {
                menu->selected_item = 0;  // Wrap to top
            }
            menu_update_scroll(menu);
            menu_system.needs_redraw = true;
            break;
            
        case MENU_NAV_SELECT:
            {
                menu_item_t* item = &menu->items[menu->selected_item];
                switch (item->type) {
                    case MENU_ITEM_ACTION:
                        if (item->action) {
                            item->action();
                        }
                        break;
                        
                    case MENU_ITEM_SUBMENU:
                        if (item->submenu) {
                            menu_set_active(item->submenu);
                        }
                        break;
                        
                    case MENU_ITEM_BACK:
                        if (menu->parent) {
                            menu_set_active(menu->parent);
                        } else {
                            menu_close();
                        }
                        break;
                }
            }
            break;
            
        case MENU_NAV_BACK:
            if (menu->parent) {
                menu_set_active(menu->parent);
            } else {
                menu_close();
            }
            break;
    }
}

void menu_set_active(menu_t* menu) {
    if (!menu) return;
    
    menu_system.current_menu = menu;
    menu_system.menu_active = true;
    menu_system.needs_redraw = true;
    
    // Set root menu if not set
    if (!menu_system.root_menu) {
        menu_system.root_menu = menu;
    }
}

void menu_close(void) {
    menu_system.menu_active = false;
    menu_system.current_menu = NULL;
    oled_clear_screen();  // Clear display when closing menu
}

void menu_update(void) {
    // This function can be used for animations or timed updates
    // Currently just ensures redraw if needed
    if (menu_system.needs_redraw && menu_system.menu_active) {
        menu_draw();
        menu_system.needs_redraw = false;
    }
}

void menu_draw(void) {
    if (!menu_system.current_menu || !menu_system.menu_active) return;
    
    menu_t* menu = menu_system.current_menu;
    
    // Clear screen
    oled_clear_screen();
    
    // Draw title
    oled_print_string(menu->title, 0, 0);
    
    // Draw menu items
    uint8_t display_line = 0;
    for (uint8_t i = menu->scroll_offset; 
         i < menu->item_count && display_line < MENU_DISPLAY_LINES; 
         i++, display_line++) {
        
        menu_draw_item(menu, i, display_line);
        
        // Draw cursor for selected item
        if (i == menu->selected_item) {
            menu_draw_cursor(display_line);
        }
    }
    
    // Draw scroll indicators if needed
    if (menu->scroll_offset > 0) {
        oled_print_char('^', 120, 1);  // Up arrow
    }
    if (menu->scroll_offset + MENU_DISPLAY_LINES < menu->item_count) {
        oled_print_char('v', 120, 7);  // Down arrow
    }
}

void menu_handle_joystick(void) {
    if (!menu_system.menu_active) return;
    
    // Get joystick position
    joystick_pos_t joy = joystick_get_position();
    
    // Simple debouncing based on time and significant movement
    static uint32_t debounce_timer = 0;
    debounce_timer++;
    
    if (debounce_timer < INPUT_DEBOUNCE_MS) return;
    
    // Handle vertical navigation
    if (joy.y > 80 && last_joystick_y <= 80) {  // Moved up
        menu_navigate(MENU_NAV_UP);
        debounce_timer = 0;
    } else if (joy.y < 20 && last_joystick_y >= 20) {  // Moved down
        menu_navigate(MENU_NAV_DOWN);
        debounce_timer = 0;
    }
    
    // Handle button press (placeholder - will be implemented when button is available)
    bool button_pressed = (joy.button != 0);  // Assuming button field exists
    if (button_pressed && !button_pressed_last) {
        menu_navigate(MENU_NAV_SELECT);
        debounce_timer = 0;
    }
    
    last_joystick_y = joy.y;
    button_pressed_last = button_pressed;
}

bool menu_is_active(void) {
    return menu_system.menu_active;
}

uint8_t menu_get_selected_position(void) {
    if (!menu_system.current_menu) return 0;
    return menu_system.current_menu->selected_item;
}
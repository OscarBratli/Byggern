#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdbool.h>

// Menu configuration constants - ULTRA MINIMAL for ATmega162
#define MAX_MENU_ITEMS     4    // Maximum items per menu 
#define MAX_MENU_TEXT_LEN  8    // Maximum length of menu item text
#define MENU_DISPLAY_LINES 4    // How many menu lines fit on OLED

// Menu item types
typedef enum {
    MENU_ITEM_ACTION,    // Executes a function when selected
    MENU_ITEM_SUBMENU,   // Opens a sub-menu
    MENU_ITEM_BACK       // Returns to parent menu
} menu_item_type_t;

// Forward declarations
typedef struct menu_t menu_t;
typedef struct menu_item_t menu_item_t;

// Menu item action function pointer
typedef void (*menu_action_func_t)(void);

// Menu item structure
struct menu_item_t {
    char text[MAX_MENU_TEXT_LEN];        // Display text
    menu_item_type_t type;               // Item type
    union {
        menu_action_func_t action;       // Function to call (ACTION type)
        menu_t* submenu;                 // Pointer to submenu (SUBMENU type)
    };
};

// Menu structure
struct menu_t {
    char title[MAX_MENU_TEXT_LEN];       // Menu title
    menu_item_t items[MAX_MENU_ITEMS];   // Menu items
    uint8_t item_count;                  // Number of items in menu
    uint8_t selected_item;               // Currently selected item index
    uint8_t scroll_offset;               // Scroll offset for long menus
    menu_t* parent;                      // Parent menu (for back navigation)
};

// Menu system state
typedef struct {
    menu_t* current_menu;                // Currently active menu
    menu_t* root_menu;                   // Root menu
    bool menu_active;                    // Is menu system active?
    bool needs_redraw;                   // Does display need update?
} menu_system_t;

// Menu navigation directions
typedef enum {
    MENU_NAV_UP,
    MENU_NAV_DOWN,
    MENU_NAV_SELECT,
    MENU_NAV_BACK
} menu_navigation_t;

// === Public Menu Functions ===

// Initialize menu system
void menu_init(void);

// Create a new menu
menu_t* menu_create(const char* title, menu_t* parent);

// Add menu items
void menu_add_action_item(menu_t* menu, const char* text, menu_action_func_t action);
void menu_add_submenu_item(menu_t* menu, const char* text, menu_t* submenu);
void menu_add_back_item(menu_t* menu);

// Menu navigation
void menu_navigate(menu_navigation_t direction);
void menu_set_active(menu_t* menu);
void menu_close(void);

// Menu display
void menu_update(void);
void menu_draw(void);

// Menu input handling (call from main loop)
void menu_handle_joystick(void);

// Get menu system state
bool menu_is_active(void);
uint8_t menu_get_selected_position(void);

// === Example Setup Function ===
void menu_setup_example(void);

#endif
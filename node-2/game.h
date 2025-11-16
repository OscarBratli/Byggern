/**
 * @file game.h
 * @brief Minimal Game System for Node 2
 */

#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING
} game_state_t;

void game_init(void);
void game_loop(void);

#endif // GAME_H

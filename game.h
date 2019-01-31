#pragma once

#include "screen.h"

struct game_t;
typedef struct game_t game_t;

/// Called to initialize internal game structures.
/// Your game should not use global variables, but should associate all state
/// with the pointer returned by game_init. This pointer will be passed to the
/// opther game functions.
///
/// A pointer to the screen to use for output/input (see screen.h) is passed.
/// The game does not own this pointer, i.e. it should not free it when the
/// game is destroyed.
game_t * game_init (screen_t * scr);

/// Called when the game is over. Free any internal state here.
void game_done (game_t * g);

/// Called when a key was pressed; hor and ver indicate which key according to
/// the table below:
///  
///   key     hor    ver
///  left     -1     0
///  right     1     0
///  up        0    -1
///  down      0     1
///
/// Return 0 if delay before calling game_tick again should remain unmodified
/// If non-zero, the return
/// value is taken as the delay before game_tick should be called again
/// (ignoring any remaining delay that remained before the key was pressed)
unsigned long game_key(game_t * g, int hor, int ver);

/// Called at regular intervals; The return code should be how long (in ms) 
/// to sleep until this function should be called again, or 0 if the game is
/// over.
unsigned long game_tick (game_t * g);


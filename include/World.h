#ifndef WORLD_H
#define WORLD_H

#include "Game.h"
#include "Playfield.h"
#include "Utility.h"
#include "Script.h"

/* Add this later | void world_to_screen(Game *game, float *sc, float *wc); */
void screen_to_world(Game *game, float *sc, float *wc);
void scroll_callback(GLFWwindow *window, double xscroll, double yscroll);
void update_pan_zoom(Game *game);
void update_edit(Game *game, Playfield *playfield, Script *script, uint32_t cell_type_index);
void update_circle_edit(Game *game, Playfield *playfield, Script *script, uint32_t cell_type_index, float radius);

#endif //WORLD_H

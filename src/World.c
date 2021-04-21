#include "World.h"

void screen_to_world(Game *game, float *sc, float *wc) {
    wc[0] = (float)sc[0] / game->transform.scale - game->transform.position.x;
    wc[1] = (float)sc[1] / game->transform.scale - game->transform.position.y;
}

void scroll_callback(GLFWwindow *window, double xscroll, double yscroll) {
    Game *game = (Game*)glfwGetWindowUserPointer(window);
    (void)xscroll;

    if(game->use_circle && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        game->circle_radius += yscroll;

        if(game->circle_radius < 0) {
            game->circle_radius = 0;
        }
    } else {
        //Get the last mouse coordinates
        float last_world[2];
        update_mouse(game);

        screen_to_world(game, game->mouse_pos, last_world);

        //Basically, don't subtract from scroll if it will become zero or negative
        game->transform.scale += game->transform.scale > 0 ? -yscroll >= game->transform.scale ? 0 : yscroll : yscroll > 0 ? yscroll : 0;

        //Get the current world coordinates
        float curr_world[2];
        screen_to_world(game, game->mouse_pos, curr_world);

        //Move the world so the mouse stays in the same position
        game->transform.position.x -= (last_world[0] - curr_world[0]);
        game->transform.position.y -= (last_world[1] - curr_world[1]);
    }
}

void update_pan_zoom(Game *game) {
    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS && !game->middle_mouse_down) {
        game->middle_mouse_down = 1;

        update_mouse(game);

        game->mouse_last[0] = game->mouse_pos[0];
        game->mouse_last[1] = game->mouse_pos[1];
    } else if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS) {
        update_mouse(game);

        game->transform.position.x += (game->mouse_pos[0] - game->mouse_last[0]) / game->transform.scale;
        game->transform.position.y += (game->mouse_pos[1] - game->mouse_last[1]) / game->transform.scale;

        game->mouse_last[0] = game->mouse_pos[0];
        game->mouse_last[1] = game->mouse_pos[1]; 
    } else {
        game->middle_mouse_down = 0;
    }
}

int point_in_field(float *point, Playfield *playfield) {
    return point[0] > -(float)(playfield->width/2) && point[0] < (float)(playfield->width/2) 
    && point[1] > -(float)(playfield->height/2) && point[1] < (float)(playfield->height/2);
}

void update_edit(Game *game, Playfield *playfield, Script *script, uint32_t cell_type_index) {
    update_mouse(game);

    //Get world coordinates
    float mouse_world[2];
    screen_to_world(game, game->mouse_pos, mouse_world);
    
    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !game->left_mouse_down) {
        game->left_mouse_down = 1;

        if(point_in_field(mouse_world, playfield)) {
            int x_index = (int)((mouse_world[0] + playfield->width/2));
            int y_index = (int)((playfield->height/2 - mouse_world[1]));

            if(script->opt_dimensions == 1) {
                if(y_index == 0) {
                    int new_cell_index = (playfield->cell_field[x_index] + 1) % script->num_cell_types;
                    playfield->cell_field[x_index] = new_cell_index;
                    playfield->field[x_index] = script->cell_types[new_cell_index];
                }
            } else {
                int index = x_index + y_index * playfield->width;
                int new_cell_index = (playfield->cell_field[index] + 1) % script->num_cell_types;
                playfield->cell_field[index] = new_cell_index;
                playfield->field[index] = script->cell_types[new_cell_index];
            }
        }

    } else if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS) {
        game->left_mouse_down = 0;
    }

    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        if(point_in_field(mouse_world, playfield)) {
            int x_index = (int)((mouse_world[0] + playfield->width/2));
            int y_index = (int)((playfield->height/2 - mouse_world[1]));
            if(script->opt_dimensions == 1) {
                if(y_index == 0) {
                    playfield->cell_field[x_index] = cell_type_index;
                    playfield->field[x_index] = script->cell_types[cell_type_index];
                }
            } else {
                int index = x_index + y_index * playfield->width;
                playfield->cell_field[index] = cell_type_index;
                playfield->field[index] = script->cell_types[cell_type_index];
            }
        }
    }
}

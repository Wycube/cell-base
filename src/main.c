#include <stdio.h>

#include "Playfield.h"
#include "Utility.h"
#include "Game.h"
#include "Text.h"

#include <string.h>
#include <stdlib.h>

//TODO:
// - Change the coordinates to a vector struct
// - Move a lot of this into another file

const uint32_t BLACK = 0x000000ff;
const uint32_t WHITE = 0xffffffff;

void next_gen(uint32_t *field, int width, int height) {
    uint32_t *buffer = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
    memcpy(buffer, field, sizeof(uint32_t) * width * height);

    int x, y;
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            int neighbors = 0;

            //Accumulate neighbors
            neighbors += (x - 1) >= 0 && (y - 1) >= 0 ? buffer[(x-1) + (y-1) * width] == BLACK ? 1 : 0 : 0;
            neighbors += (y - 1) >= 0 ? buffer[x + (y-1) * width] == BLACK ? 1 : 0 : 0;
            neighbors += (x + 1) < width && (y - 1) >= 0 ? buffer[(x+1) + (y-1) * width] == BLACK ? 1 : 0 : 0;
            neighbors += (x - 1) >= 0 ? buffer[(x-1) + y * width] == BLACK ? 1 : 0 : 0;
            neighbors += (x + 1) < width ? buffer[(x+1) + y * width] == BLACK ? 1 : 0 : 0;
            neighbors += (x - 1) >= 0 && (y + 1) < height ? buffer[(x-1) + (y+1) * width] == BLACK ? 1 : 0 : 0;
            neighbors += (y + 1) < height ? buffer[x + (y+1) * width] == BLACK ? 1 : 0 : 0;
            neighbors += (x + 1) < width && (y + 1) < height ? buffer[(x+1) + (y+1) * width] == BLACK ? 1 : 0 : 0;

            //Apply rules
            if(buffer[x + y * width] == BLACK) {
                //Alive
                if(neighbors == 2 || neighbors == 3) {
                    field[x + y * width] = BLACK;
                } else {
                    field[x + y * width] = WHITE;
                }
            } else {
                if(neighbors == 3) {
                    field[x + y * width] = BLACK;
                } else {
                    field[x + y * width] = WHITE;
                }
            }
        }
    }

    free(buffer);
}

/*
//TODO: Add a struct containing information about the current cell color
//and other configuration data.
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    (void)mods;
	uint32_t *field = (uint32_t*)glfwGetWindowUserPointer(window);

	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//Set the pixel there
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		//Tranform the coordinates
		int sx, sy;
		sx = x / 20;
		sy = y / 20;

		uint32_t color = field[sx + sy * 20];

		if(color == 0xffffffff) {
			field[sx + sy * 20] = 0x000000ff;
		} else {
			field[sx + sy * 20] = 0xffffffff;
		}
	}	
}
*/

void screen_to_world(Game *game, double *sc, float *wc) {
    wc[0] = (float)sc[0] / game->transform.scale - game->transform.position.x;
    wc[1] = (float)sc[1] / game->transform.scale - game->transform.position.y;
}

void scroll_callback(GLFWwindow *window, double xscroll, double yscroll) {
    Game *game = (Game*)glfwGetWindowUserPointer(window);
    (void)xscroll;
    yscroll *= 0.5;

    //Get the last mouse coordinates
    double mouse[2];
    float last_world[2];
    glfwGetCursorPos(window, &mouse[0], &mouse[1]);
    screen_to_world(game, mouse, last_world);

    //Basically, don't subtract from scroll if it will become zero or negative
    game->transform.scale += game->transform.scale > 0 ? -yscroll >= game->transform.scale ? 0 : yscroll : yscroll > 0 ? yscroll : 0;

    //Get the current world coordinates
    float curr_world[2];
    screen_to_world(game, mouse, curr_world);

    //Move the world so the mouse stays in the same position
    game->transform.position.x -= (last_world[0] - curr_world[0]);
    game->transform.position.y -= (last_world[1] - curr_world[1]);
}

void update_pan_zoom(Game *game, float *mouse_last, int *mouse_down) {
    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS && !*mouse_down) {
        *mouse_down = 1;

        double mouse_x, mouse_y;
        glfwGetCursorPos(game->window, &mouse_x, &mouse_y);

        mouse_last[0] = (float)mouse_x;
        mouse_last[1] = (float)mouse_y;
    } else if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(game->window, &mouse_x, &mouse_y);

        game->transform.position.x += ((float)mouse_x - mouse_last[0]) / game->transform.scale;
        game->transform.position.y += ((float)mouse_y - mouse_last[1]) / game->transform.scale;

        mouse_last[0] = (float)mouse_x;
        mouse_last[1] = (float)mouse_y; 
    } else {
        *mouse_down = 0;
    }
}

int point_in_field(float *point, Playfield *playfield) {
    return point[0] > -(float)(playfield->width/2) && point[0] < (float)(playfield->width/2) 
    && point[1] > -(float)(playfield->height / 2) && point[1] < (float)(playfield->height/2);
}

void update_edit(Game *game, Playfield *playfield, int *left_mouse_down, uint32_t *mouse_color) {
    double mouse_pos[2];
    glfwGetCursorPos(game->window, &mouse_pos[0], &mouse_pos[1]);

    //Get world coordinates
    float mouse_world[2];
    screen_to_world(game, mouse_pos, mouse_world);
    
    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !*left_mouse_down) {
        *left_mouse_down = 1;

        if(point_in_field(mouse_world, playfield)) {
            int x_index = (int)((mouse_world[0] + playfield->width/2));
            int y_index = (int)((playfield->height/2 - mouse_world[1]));
            int index = x_index + y_index * playfield->width;
            playfield->field[index] = playfield->field[index] == BLACK ? WHITE : BLACK;
        }

    } else if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_1) != GLFW_PRESS) {
        *left_mouse_down = 0;
    }

    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        if(point_in_field(mouse_world, playfield)) {
            int x_index = (int)((mouse_world[0] + playfield->width/2));
            int y_index = (int)((playfield->height/2 - mouse_world[1]));
            int index = x_index + y_index * playfield->width;
            playfield->field[index] = *mouse_color;
        }
    }
}

int main() {
    int width = 400, height = 500;
    int fwidth = 50, fheight = 50;

    Game game = create_game(width, height, "Cellular Automata");
    init_opengl_objects();

    glfwSetWindowUserPointer(game.window, &game);
    glfwSetScrollCallback(game.window, scroll_callback);

    Playfield playfield = create_playfield(fwidth, fheight);
    game.transform.position.x = fwidth / 2;
    game.transform.position.y = fheight / 2 + fheight / 4;
    game.transform.scale = (float)width / (float)fwidth;
    memset(playfield.field, WHITE, sizeof(uint32_t) * playfield.width * playfield.height);
    
    //Generate checker board pattern
    uint32_t x, y;
    for(y = 0; y < playfield.height; y++) {
        for(x = 0; x < playfield.width; x++) {
            playfield.field[x + y * playfield.width] = (x + y % 2) % 2 == 1 ? WHITE : BLACK;
        }
    }
    
    FontInfo font = create_font();

    int generation = 0;
    float mouse_last[2];
    int middle_mouse_down = 0, left_mouse_down = 0, key_down = 0;
    int play = 0;
    uint32_t mouse_color = 0x000000ff;
    uint32_t colors[4] = {0x000000ff, 0xffffffff, 0xff0000ff, 0x00ff00ff};
    char buffer[40];

    while(!glfwWindowShouldClose(game.window)) {
        glfwPollEvents();

        glfwGetWindowSize(game.window, &game.width, &game.height);

        if(glfwGetKey(game.window, GLFW_KEY_R) == GLFW_PRESS) {
            play = 1;
        } else if(glfwGetKey(game.window, GLFW_KEY_T) == GLFW_PRESS) {
            play = 0;
        }

        if(( glfwGetKey(game.window, GLFW_KEY_P) == GLFW_PRESS && !key_down) || play) {
            key_down = 1;
            next_gen(playfield.field, playfield.width, playfield.height);
            generation++;
        } else if(glfwGetKey(game.window, GLFW_KEY_P) != GLFW_PRESS) {
            key_down = 0;
        }

        if(glfwGetKey(game.window, GLFW_KEY_1) == GLFW_PRESS) {
            mouse_color = colors[0];
        }
        if(glfwGetKey(game.window, GLFW_KEY_2) == GLFW_PRESS) {
            mouse_color = colors[1];
        }
        if(glfwGetKey(game.window, GLFW_KEY_3) == GLFW_PRESS) {
            mouse_color = colors[2];
        }
        if(glfwGetKey(game.window, GLFW_KEY_4) == GLFW_PRESS) {
            mouse_color = colors[3];
        }

        update_pan_zoom(&game, mouse_last, &middle_mouse_down);
        update_edit(&game, &playfield, &left_mouse_down, &mouse_color);

        glViewport(0, 0, game.width, game.height);
        glClear(GL_COLOR_BUFFER_BIT);

        draw_game(&game, playfield.field, playfield.width, playfield.height);

        double mouse_screen[2];
        float mouse_world[2];
        glfwGetCursorPos(game.window, &mouse_screen[0], &mouse_screen[1]);
        screen_to_world(&game, mouse_screen, mouse_world);

        sprintf(buffer, "Mouse World:(%.2f, %.2f)", mouse_world[0], mouse_world[1]);
        draw_string(font, buffer, 5, 20, 0x000000ff, game.width, game.height, 20);
        sprintf(buffer, "Scale:%.1f, Pos:(%.2f, %.2f)", game.transform.scale, game.transform.position.x, game.transform.position.y);
        draw_string(font, buffer, 5, 40, 0xaaaaaaff, game.width, game.height, 20);
        sprintf(buffer, "Generation: %i", generation);
        draw_string(font, buffer, 5, 60, 0x0000ffff, game.width, game.height, 20);
        sprintf(buffer, "Color: %08X", mouse_color);
        draw_string(font, buffer, 5, 80, mouse_color, game.width, game.height, 20);

        glfwSwapBuffers(game.window);
    }

    destroy_game(&game);

    return 0;
}

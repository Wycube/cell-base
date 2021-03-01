#include <stdio.h>

#include "Playfield.h"
#include "Utility.h"
#include "Game.h"
#include "Text.h"

#include <string.h>
#include <stdlib.h>

void next_gen(uint32_t *field, int width, int height) {
    uint32_t *buffer = (uint32_t*)malloc(sizeof(uint32_t) * width * height);
    memcpy(buffer, field, sizeof(uint32_t) * width * height);

    int x, y;
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            int neighbors = 0;

            //Accumulate neighbors
            neighbors += (x - 1) >= 0 && (y - 1) >= 0 ? buffer[(x-1) + (y-1) * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (y - 1) >= 0 ? buffer[x + (y-1) * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (x + 1) < width && (y - 1) >= 0 ? buffer[(x+1) + (y-1) * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (x - 1) >= 0 ? buffer[(x-1) + y * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (x + 1) < width ? buffer[(x+1) + y * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (x - 1) >= 0 && (y + 1) < height ? buffer[(x-1) + (y+1) * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (y + 1) < height ? buffer[x + (y+1) * width] == 0x000000ff ? 1 : 0 : 0;
            neighbors += (x + 1) < width && (y + 1) < height ? buffer[(x+1) + (y+1) * width] == 0x000000ff ? 1 : 0 : 0;

            //Apply rules
            if(buffer[x + y * width] == 0x000000ff) {
                //Alive
                if(neighbors == 2 || neighbors == 3) {
                    field[x + y * width] = 0x000000ff;
                } else {
                    field[x + y * width] = 0xffffffff;
                }
            } else {
                if(neighbors == 3) {
                    field[x + y * width] = 0x000000ff;
                } else {
                    field[x + y * width] = 0xffffffff;
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

void scroll_callback(GLFWwindow *window, double xscroll, double yscroll) {
    Game *game = (Game*)glfwGetWindowUserPointer(window);
    (void)xscroll;
    //Basically, don't subtract from scroll if it will become zero or negative
    game->transform.scale += game->transform.scale > 0 ? -yscroll >= game->transform.scale ? 0 : yscroll : yscroll > 0 ? yscroll : 0;
}

void update_pan(Game *game, float *pos_last, float *mouse_last, int *mouse_down) {
    if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS && !*mouse_down) {
        *mouse_down = 1;

        double mouse_x, mouse_y;
        glfwGetCursorPos(game->window, &mouse_x, &mouse_y);

        mouse_last[0] = (float)mouse_x;
        mouse_last[1] = (float)mouse_y;
        pos_last[0] = game->transform.position.x;
        pos_last[1] = game->transform.position.y;
    } else if(glfwGetMouseButton(game->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(game->window, &mouse_x, &mouse_y);

        double diff[2];
        diff[0] = mouse_last[0] - mouse_x;
        diff[1] = mouse_last[1] - mouse_y;

        //Not really sure why I have to multiply by 2 here but I do
        game->transform.position.x = pos_last[0] - (diff[0] / game->transform.scale * 2.0f);
        game->transform.position.y = pos_last[1] + (diff[1] / game->transform.scale * 2.0f);
    } else {
        *mouse_down = 0;
    }
}

int main() {
    int width = 400, height = 500;

    Game game = create_game(width, height, "Cellular Automata");
    game.transform.scale = 20.0f;
    game.transform.position.y = 5.0f;
    init_opengl_objects();

    Playfield playfield = create_playfield(20, 20);

    memset(playfield.field, rgb_to_int(255, 255, 255), sizeof(uint32_t) * playfield.width * playfield.height);

    playfield.field[5 + 5 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[6 + 6 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[6 + 7 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[5 + 7 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[4 + 7 * playfield.width] = rgb_to_int(0, 0, 0);

    glfwSetWindowUserPointer(game.window, &game);
    glfwSetScrollCallback(game.window, scroll_callback);
    //glfwSetMouseButtonCallback(game.window, mouse_button_callback);
    //glfwSetWindowUserPointer(game.window, playfield.field);

    //Generate checker board pattern

    uint32_t x, y;
    for(y = 0; y < playfield.width; y++) {
        for(x = 0; x < playfield.height; x++) {
            playfield.field[x + y * playfield.width] = (x + y % 2) % 2 == 1 ? rgb_to_int(255, 255, 255) : rgb_to_int(0, 0, 0);
        }
    }
    
    FontInfo font = create_font();

    int generation = 0;

    float mouse_last[2];
    float pos_last[2];
    int mouse_down = 0;

    while(!glfwWindowShouldClose(game.window)) {
        glfwPollEvents();

        glfwGetWindowSize(game.window, &game.width, &game.height);

        if(glfwGetKey(game.window, GLFW_KEY_P) == GLFW_PRESS) {
            next_gen(playfield.field, playfield.width, playfield.height);
            generation++;
        }

        update_pan(&game, pos_last, mouse_last, &mouse_down);

        glViewport(0, 0, game.width, game.height);
        glClear(GL_COLOR_BUFFER_BIT);
        draw_game(&game, playfield.field, playfield.width, playfield.height);
        draw_string(font, "Yeet Yeet Boooi!", 5, 420, 0xaaaaaaff, game.width, game.height, 20);
        draw_string(font, "WOW much text", 5, 460, 0x0000ffff, game.width, game.height, 20);

        glfwSwapBuffers(game.window);
    }

    destroy_game(&game);

    return 0;
}

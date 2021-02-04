#include <stdio.h>

#include "Playfield.h"
#include "Utility.h"
#include "Game.h"

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

int main() {
    Game game = create_game(400, 400, "Cellular Automata");
    init_opengl_objects();

    Playfield playfield = create_playfield(20, 20);

    memset(playfield.field, rgb_to_int(255, 255, 255), sizeof(uint32_t) * playfield.width * playfield.height);

    playfield.field[5 + 5 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[6 + 6 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[6 + 7 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[5 + 7 * playfield.width] = rgb_to_int(0, 0, 0);
    playfield.field[4 + 7 * playfield.width] = rgb_to_int(0, 0, 0);

    //Generate checker board pattern
    
    uint32_t x, y;
    for(y = 0; y < playfield.width; y++) {
        for(x = 0; x < playfield.height; x++) {
            playfield.field[x + y * playfield.width] = (x + y % 2) % 2 == 1 ? rgb_to_int(255, 255, 255) : rgb_to_int(0, 0, 0);
        }
    }
    


    while(!glfwWindowShouldClose(game.window)) {
        glfwPollEvents();

        if(glfwGetKey(game.window, GLFW_KEY_P) == GLFW_PRESS) {
            next_gen(playfield.field, playfield.width, playfield.height);
        }

        draw_texture(playfield.field, playfield.width, playfield.height);

        glfwSwapBuffers(game.window);
    }

    destroy_game(&game);

    return 0;
}

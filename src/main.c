#include <stdio.h>

#include "Text.h"
#include "World.h"
#include "Script.h"

#include <string.h>
#include <stdlib.h>

//TODO:
// - Change the coordinates to a vector struct
// - Move a lot of this into another file

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


int main(int argc, char *argv[]) {
    int width = 400, height = 500;
    int fwidth = 50, fheight = 50;

    Script script;

    if(argc > 1) {
        char buffer[80];
        sprintf(buffer, "D:/Spencer/dev/C/cell-base/lua/%s", argv[1]);
        script = get_script(buffer);
    } else {
        script = get_script("D:/Spencer/dev/C/cell-base/lua/game_of_life.lua");
    }

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
            playfield.field[x + y * playfield.width] = script.cell_types[0];//(x + y % 2) % 2 == 1 ? script.cell_types[0] : script.cell_types[1];
            playfield.cell_field[x + y * playfield.width] = 0;//(x + y % 2) % 2 == 1 ? 0 : 1;
        }
    }
    
    FontInfo font = create_font();

    int key_down = 0;
    int play = 0;
    uint32_t edit_cell_type = 1;
    char buffer[40];
    char *running_text = "stopped";

    while(!glfwWindowShouldClose(game.window)) {
        glfwPollEvents();

        glfwGetWindowSize(game.window, &game.width, &game.height);
        glViewport(0, 0, game.width, game.height);

        if(glfwGetKey(game.window, GLFW_KEY_R) == GLFW_PRESS) {
            play = 1;
        } else if(glfwGetKey(game.window, GLFW_KEY_T) == GLFW_PRESS) {
            play = 0;
        }

        if(( glfwGetKey(game.window, GLFW_KEY_P) == GLFW_PRESS && !key_down) || play) {
            key_down = 1;
            //next_gen(playfield.field, playfield.width, playfield.height);
            run_cell_update(&script, &playfield);
            script.generation++;
            running_text = "running";
        } else if(glfwGetKey(game.window, GLFW_KEY_P) != GLFW_PRESS) {
            key_down = 0;
            running_text = "stopped";
        }

        if(glfwGetKey(game.window, GLFW_KEY_1) == GLFW_PRESS) {
            edit_cell_type = 0;
        }
        if(glfwGetKey(game.window, GLFW_KEY_2) == GLFW_PRESS) {
            edit_cell_type = 1;
        }
        if(glfwGetKey(game.window, GLFW_KEY_3) == GLFW_PRESS && script.num_cell_types > 2) {
            edit_cell_type = 2;
        }
        if(glfwGetKey(game.window, GLFW_KEY_4) == GLFW_PRESS && script.num_cell_types > 3) {
            edit_cell_type = 3;
        }

        update_pan_zoom(&game);
        update_edit(&game, &playfield, &script, edit_cell_type);

        glClear(GL_COLOR_BUFFER_BIT);

        draw_game(&game, playfield.field, playfield.width, playfield.height);

        
        float mouse_world[2];

        update_mouse(&game);
        screen_to_world(&game, game.mouse_pos, mouse_world);

        draw_string(font, running_text, 5, 20, 0x000000ff, game.width, game.height, 20);
        sprintf(buffer, "Scale:%.1f, Pos:(%.2f, %.2f)", game.transform.scale, game.transform.position.x, game.transform.position.y);
        draw_string(font, buffer, 5, 40, 0xaaaaaaff, game.width, game.height, 20);
        sprintf(buffer, "Generation: %i", script.generation);
        draw_string(font, buffer, 5, 60, 0x0000ffff, game.width, game.height, 20);
        sprintf(buffer, "Color: %08X", script.cell_types[edit_cell_type]);
        draw_string(font, buffer, 5, 80, script.cell_types[edit_cell_type], game.width, game.height, 20);

        glfwSwapBuffers(game.window);
    }

    destroy_game(&game);
    free_script(&script);

    return 0;
}

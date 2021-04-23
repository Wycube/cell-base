#include <stdio.h>

#include "Text.h"
#include "World.h"
#include "Script.h"
#include "Circle.h"

#include <string.h>
#include <stdlib.h>

//Far-Reaching (and some not that far-reaching) Goals:
// - Hexagonal Cellular automata
// - Regular cellular automata that can modify cells other than itself
// - 3D cellular automata ??
// - Optimize / Parrelleize
// - Add life-like cellular automata rules and other rule notations
// - Elementary cellular automata rule notation
// - Ant-like, or termite-like, cellular automata options
// - Color defined by 0 - 255 as well
// - User data specified in lua and stored in script struct, passed to and returned from lua update function
// - Add a resize callback to make window resizing SMOOTH

int main(int argc, char *argv[]) {
    int width = 400, height = 500;

    Script script;

    if(argc > 1) {
        char buffer[80];
        sprintf(buffer, "D:/Spencer/dev/C/cell-base/examples/%s", argv[1]);
        script = get_script(buffer);
    } else {
        script = get_script("D:/Spencer/dev/C/cell-base/examples/game_of_life.lua");
    }

    Game game = create_game(width, height, "Cellular Automata");
    init_opengl_objects();
    init_circle_draw();

    glfwSetWindowUserPointer(game.window, &game);
    glfwSetScrollCallback(game.window, scroll_callback);

    Playfield playfield = create_playfield(script.opt_field_width, script.opt_field_height);
    game.transform.position.x = script.opt_field_width / 2;
    game.transform.position.y = script.opt_field_height / 2 + script.opt_field_height / 4;
    game.transform.scale = (float)width / (float)script.opt_field_width;
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

    int key_down = 0, circle_key_down = 0;
    int play = 0;
    uint32_t edit_cell_type = 0;
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

        if((glfwGetKey(game.window, GLFW_KEY_P) == GLFW_PRESS && !key_down) || play) {
            key_down = 1;
            run_cell_update(&script, &playfield);
            script.generation++;
            running_text = "running";
        } else if(glfwGetKey(game.window, GLFW_KEY_P) != GLFW_PRESS) {
            key_down = 0;
            running_text = "stopped";
        }

        for(int i = 0; i < script.num_cell_types; i++) {
            if(glfwGetKey(game.window, GLFW_KEY_1 + i)) {
                edit_cell_type = i;
            }
        }

        if(glfwGetKey(game.window, GLFW_KEY_E) == GLFW_PRESS && !circle_key_down) {
            circle_key_down = true;
            game.use_circle = !game.use_circle;
        } else if(glfwGetKey(game.window, GLFW_KEY_E) != GLFW_PRESS) {
            circle_key_down = false;
        }

        update_pan_zoom(&game);

        if(game.use_circle) {
            update_circle_edit(&game, &playfield, &script, edit_cell_type, game.circle_radius);
        } else {
            update_edit(&game, &playfield, &script, edit_cell_type);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        draw_game(&game, playfield.field, playfield.width, playfield.height);

        
        float mouse_world[2];

        update_mouse(&game);
        screen_to_world(&game, game.mouse_pos, mouse_world);

        if(game.use_circle) {
            Circle display_circle = {game.mouse_pos[0], game.mouse_pos[1], game.circle_radius * game.transform.scale};
            draw_circle(&display_circle, game.width, game.height, script.cell_types[edit_cell_type]);
        }

        draw_string(font, running_text, 5, 20, 0x000000ff, game.width, game.height, 20);
        sprintf(buffer, "Scale:%.1f, Pos:(%.2f, %.2f)", game.transform.scale, game.transform.position.x, game.transform.position.y);
        draw_string(font, buffer, 5, 40, 0x000000ff, game.width, game.height, 20);
        sprintf(buffer, "Circle Radius: %.1f", game.circle_radius);
        draw_string(font, buffer, 5, 60, 0x000000ff, game.width, game.height, 20);
        sprintf(buffer, "Color: %08X", script.cell_types[edit_cell_type]);
        draw_string(font, buffer, 5, 80, 0x000000ff, game.width, game.height, 20);
        draw_string(font, "                [COLOR]", 5, 80, script.cell_types[edit_cell_type], game.width, game.height, 20);
        sprintf(buffer, "Generation: %i", script.generation);
        draw_string(font, buffer, 5, 100, 0x000000ff, game.width, game.height, 20);

        glfwSwapBuffers(game.window);
    }

    destroy_game(&game);
    free_script(&script);

    return 0;
}

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

//Holds the glfw window instance and some other stuff.
typedef struct {
    GLFWwindow *window;
    int width, heigth;
} Game;

Game create_game(int width, int height, const char *title);
void destroy_game(Game *game);

void init_opengl_objects();
void draw_texture(uint32_t *tex, int width, int height);

#endif //GAME_H

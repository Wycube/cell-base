#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

typedef struct {
    float scale;
    struct {
        float x;
        float y;
    } position;
} Transfrom;

//Holds the glfw window instance and some other stuff.
typedef struct {
    GLFWwindow *window;
    int width, height;
    Transfrom transform;
} Game;

Game create_game(int width, int height, const char *title);
void destroy_game(Game *game);

void init_opengl_objects();
void draw_game(Game *game, uint32_t *tex, int width, int height);

#endif //GAME_H

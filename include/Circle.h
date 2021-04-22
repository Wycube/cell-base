#ifndef CIRCLE_H
#define CIRCLE_H

#include <stdint.h>

typedef struct {
    float x, y;
    float radius;
} Circle;

Circle create_circle(float x, float y, float radius);
void draw_circle(Circle *circle, int width, int height, uint32_t color);
void init_circle_draw();

#endif //CIRCLE_H

#ifndef EDIT_H
#define EDIT_H


typedef struct {
    float x, y;
    float radius;
} Circle;

Circle create_circle(float x, float y, float radius);
void draw_circle(Circle *circle, int width, int height);
void init_circle_draw();

#endif //EDIT_H

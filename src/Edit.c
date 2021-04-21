#include "Edit.h"

#include <glad/glad.h>
#include <stdio.h>

const char *circle_vertex = 
"#version 120\n"
"attribute vec2 pos;\n"
"varying vec2 frag_pos;\n"
"\n"
"void main() {\n"
"   gl_Position = vec4(pos, 0.0, 1.0);\n"
"   frag_pos = pos * vec2(1.0, -1.0);\n"
"}";

const char *circle_fragment = 
"#version 120\n"
"varying vec2 frag_pos;\n"
"uniform vec2 center;\n"
"uniform vec2 dimensions;\n"
"uniform float radius;\n"
"float thickness = 1;\n"
"\n"
"vec2 gl_to_screen(vec2 position) {"
"   return ((position + 1) / 2) * dimensions;"
"}"
"\n"
"void main() {\n"
"   vec2 pos = gl_to_screen(frag_pos);\n"
"   vec2 diff = center - pos;\n"
"   float len = length(diff);\n"
"   if(len >= radius - thickness && len <= radius + thickness) {\n"
"       gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
"   } else {\n"
"       gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);\n"
"   }\n"
"}";

unsigned int circle_vbo = 0;
unsigned int circle_program = 0;

Circle create_circle(float x, float y, float radius) {
    Circle circle = {x, y, radius};
    return circle;
}

void draw_circle(Circle *circle, int width, int height) {
    glBindBuffer(GL_ARRAY_BUFFER, circle_vbo);
    glUseProgram(circle_program);

    GLuint vertex_attrib = glGetAttribLocation(circle_program, "pos");
    glVertexAttribPointer(vertex_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    unsigned int u_center = glGetUniformLocation(circle_program, "center");
    unsigned int u_dimensions = glGetUniformLocation(circle_program, "dimensions");
    unsigned int u_radius = glGetUniformLocation(circle_program, "radius");
    glUniform2f(u_center, circle->x, circle->y);
    glUniform2f(u_dimensions, width, height);
    glUniform1f(u_radius, circle->radius);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void init_circle_draw() {
    float pos[8] = {
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f
    };

    glGenBuffers(1, &circle_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, circle_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, pos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Create vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &circle_vertex, 0);
    glCompileShader(vertex);

    //Check Compile Results
    GLint compile = GL_FALSE;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Circle Vertex shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(vertex, 80, 0, log);
        printf("%s\n", log);
    }

    //Create fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &circle_fragment, 0);
    glCompileShader(fragment);

    //Check Compile Results
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Circle Fragment shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(fragment, 80, 0, log);
        printf("%s\n", log);
    }

    //Create program, link, and use it
    circle_program = glCreateProgram();
    glAttachShader(circle_program, vertex);
    glAttachShader(circle_program, fragment);
    glLinkProgram(circle_program);
}

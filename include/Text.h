/*
 * A collection of utility functions and shaders for
 * drawing some nice crisp clean, hopefully, text using
 * multi-channel signed distance fields(MSDF) using an 
 * implementation and tool from Viktor Chlumský to generate
 * the SDFs and using their meta-data for the fragment shader.
 *
 * Hopefully this works.
 */

#ifndef TEXT_H
#define TEXT_H

#include <stdint.h>
#include <glad/glad.h>

#include "Glyph.h"

typedef struct {
    GLuint program;
    GLuint texture;
    GLuint vbo;
    const GlyphData *data;    
} FontInfo;

//Later change so the bitmap and data parts are embedded into the program
FontInfo create_font();
//More functions for drawing strings
void draw_string(FontInfo font, const char *string, int x, int y, uint32_t color, int width, int height, int size);

#endif //TEXT_H

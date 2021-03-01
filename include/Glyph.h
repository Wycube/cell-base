#ifndef GLYPH_H
#define GLYPH_H

typedef struct {
    float plane_left;
    float plane_bottom;
    float plane_right;
    float plane_top;

    float atlas_left;
    float atlas_bottom;
    float atlas_right;
    float atlas_top;
} GlyphData;


#endif //GLYPH_H

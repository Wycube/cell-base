#include "Text.h"
#include "Resources.h"
#include "Utility.h"

#include <string.h>
#include <stdio.h>

const char *text_vertex = 
"#version 120\n"
"attribute vec4 vertex;\n"
"varying vec2 texCoord;\n"
"\n"
"void main() {\n"
"   gl_Position = vec4(vertex.xy, 0.0, 1.0);\n"
"   texCoord = vertex.zw;\n"
"}";

const char *text_fragment = 
"#version 120\n"
"varying vec2 texCoord;\n"
"uniform sampler2D msdf;\n"
"uniform float magic;\n" 
"uniform vec4 color;\n"
"float width = 0.5;\n"
"float border_width = 0.5;\n"
""
"float median(float r, float g, float b) {\n"
"    return max(min(r, g), min(max(r, g), b));\n"
"}\n"
""
"void main() {\n"
"   vec3 msd = texture2D(msdf, vec2(1, -1) * texCoord.xy + vec2(0, 1)).rgb;\n"
"   float sd = magic * (median(msd.r, msd.g, msd.b) - (1.0 - width));\n"
"   float opacity = clamp(sd + (1.0 - width), 0.0, 1.0);\n"
"   gl_FragColor = vec4(color.rgb, opacity);\n"
"}";

GLuint create_texture(const unsigned char *bitmap, int width, int height) {
    //Create Texture and setup Texture stuff
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    //Setup some texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Put texture data in now
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bitmap);

    return tex;
}

GLuint create_shaders() {
    //Create vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &text_vertex, NULL);
    glCompileShader(vertex);

    //Check Compile Results
    GLint compile = GL_FALSE;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Text Vertex shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(vertex, 80, NULL, log);
        printf("%s\n", log);
    }

    //Create fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &text_fragment, NULL);
    glCompileShader(fragment);

    //Check Compile Results
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Text Fragment shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(fragment, 80, NULL, log);
        printf("%s\n", log);
    }

    //Create program, link, and use it
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    return program;
}

FontInfo create_font() {
    FontInfo info;
    info.program = create_shaders();
    info.texture = create_texture(msdf_bitmap, msdf_width, msdf_height);
    info.data = msdf_data;
    glGenBuffers(1, &info.vbo);
    return info;
}

void draw_string(FontInfo font, const char *string, int x, int y, uint32_t color, int width, int height, int size) {
    int length = strlen(string);
    
    if(length <= 0) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    glUseProgram(font.program);

    float advance = 0.0f;

    GLuint vertex_attrib = glGetAttribLocation(font.program, "vertex");
    glVertexAttribPointer(vertex_attrib, 4, GL_FLOAT, GL_FALSE, 0, 0);

    //The anti-aliasing requires some adjusting with the scale, but whatever
    static const float size_ratio = 0.42f; //Also 168/400, which is height_of_biggest_character / height_of_window
    float fhscale = (float)size / ((float)height * size_ratio);
    float fwscale = (float)size / ((float)width * size_ratio);

    GLuint u_magic = glGetUniformLocation(font.program, "magic");

    GLuint u_color = glGetUniformLocation(font.program, "color");
    float color_vec[4];
    int_to_vec4(color_vec, color);
    glUniform4f(u_color, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float cursor = ((float)x / (float)width) * 2 - 1;
    float baseline = ((float)y / (float)height) * -2 + 1;

    int i;
    for(i = 0; i < length; i++) {
        GlyphData gd = font.data[string[i] - 32];
        float g_left = gd.plane_left * fwscale;
        float g_right = gd.plane_right * fwscale;
        float g_bottom = gd.plane_bottom * fhscale;
        float g_top = gd.plane_top * fhscale;

        float magic = size / (gd.atlas_top - gd.atlas_bottom) * 2; //This is, size_in_pixels_onscreen / size_in_pixels_of_character * pixel_distance
        glUniform1f(u_magic, magic);

        float vertex[] = {
            cursor + advance + g_left,  baseline + g_top,    gd.atlas_left/188.0f,  gd.atlas_top/188.0f,    //Top-Left
            cursor + advance + g_left,  baseline + g_bottom, gd.atlas_left/188.0f,  gd.atlas_bottom/188.0f, //Bottom-Left
            cursor + advance + g_right, baseline + g_top,    gd.atlas_right/188.0f, gd.atlas_top/188.0f,    //Top-Right
            cursor + advance + g_right, baseline + g_top,    gd.atlas_right/188.0f, gd.atlas_top/188.0f,    //Top-Right, again
            cursor + advance + g_left,  baseline + g_bottom, gd.atlas_left/188.0f,  gd.atlas_bottom/188.0f, //Bottom-Left
            cursor + advance + g_right, baseline + g_bottom, gd.atlas_right/188.0f, gd.atlas_bottom/188.0f  //Bottom-Right
        };

        glUniform4f(u_color, color_vec[0], color_vec[1], color_vec[2], color_vec[3]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, vertex, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(vertex_attrib);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        advance += h_advance * fwscale;
    }

    glDisable(GL_BLEND);
}

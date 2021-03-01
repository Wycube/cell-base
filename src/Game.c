#include "Game.h"

//Super basic vertex and fragment shaders
static const char *vertex_shader = 
"#version 120\n"
"attribute vec2 pos;\n"
"varying vec2 texCoord;\n"
"uniform float width;\n"
"uniform float height;\n"
"uniform float fieldWidth;\n"
"uniform float fieldHeight;\n"
"uniform float scale;\n"
"uniform vec2 translation;\n"
""
"vec2 worldToScreen(vec2 worldPos) {\n"
"   return (worldPos + translation) * scale;\n"
"}\n"
""
"void main() {\n"
"   vec2 view_pos = worldToScreen(pos * vec2(fieldWidth, fieldHeight));\n"
"   gl_Position = vec4(view_pos / vec2(width, height), 0, 1);\n"
"   vec2 new_pos = pos * vec2(1.0f, -1.0f);\n"
"   texCoord = (new_pos + 1.0f) / 2.0f;\n"
"}";

static const char *fragment_shader = 
"#version 120\n"
"uniform sampler2D tex;\n"
"varying vec2 texCoord;\n"
"void main() {\n"
"   gl_FragColor = texture2D(tex, texCoord);\n"
"}";

//Creates a game structure and the glfw window and returns it
Game create_game(int width, int height, const char *title) {
    //Initialize glfw
    if(!glfwInit()) {
        printf("Error: GLFW failed to initialize!\n");
    }

    GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);

    //Check for success
    if(!window) {
        printf("Error: Failed to create GLFWwindow!\n");
    }

    //Do some window stuff
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    //Init glad
    if(!gladLoadGL()) {
        printf("Error: Glad failed to initialize!\n");        
    } else {
        printf("Loaded OpenGl Version %i.%i\n", GLVersion.major, GLVersion.minor);
    }

    //Create struct and return it
    Game game = {window, width, height, {1.0f, {0.0f, 0.0f}}};
    return game;
}

//Frees the glfw window
void destroy_game(Game *game) {
    glfwDestroyWindow(game->window);
    game->window = NULL;
}

//put this in a struct later
GLuint program;
GLuint tex;
GLuint vbo;

//Creates opengl shaders and program
void init_opengl_objects() {
    //Create Texture and setup Texture stuff
    tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    
    //Setup some texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Create vertex shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader, NULL);
    glCompileShader(vertex);

    //Check Compile Results
    GLint compile = GL_FALSE;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Vertex shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(vertex, 80, NULL, log);
        printf("%s\n", log);
    }

    //Create fragment shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader, NULL);
    glCompileShader(fragment);

    //Check Compile Results
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        printf("Error: Fragment shader failed to compile!\n");
        char log[80];
        glGetShaderInfoLog(fragment, 80, NULL, log);
        printf("%s\n", log);
    }

    //Create program, link, and use it
    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glUseProgram(program);

    //Create vbo for vertex positions
    float pos[] = {
         1.0f,  1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), &pos, GL_STATIC_DRAW);

    GLuint pos_attrib = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void draw_game(Game *game, uint32_t *texture, int tex_width, int tex_height) {
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Enable attribute
    GLuint pos_attrib = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // //Pass in view matrix
    // GLint u_ortho_mat = glGetUniformLocation(program, "orthoMat");
    // float mat[4];
    // mat[0] = 1.0f / game->width;
    // mat[3] = 1.0f / game->height;
    // glUniformMatrix2fv(u_ortho_mat, 1, GL_FALSE, mat);

    GLint u_width = glGetUniformLocation(program, "width");
    GLint u_height = glGetUniformLocation(program, "height");
    glUniform1f(u_width, (float)game->width);
    glUniform1f(u_height, (float)game->height);

    GLint u_field_width = glGetUniformLocation(program, "fieldWidth");
    GLint u_field_height = glGetUniformLocation(program, "fieldHeight");
    glUniform1f(u_field_width, (float)tex_width);
    glUniform1f(u_field_height, (float)tex_height);

    GLint u_scale = glGetUniformLocation(program, "scale");
    GLint u_translation = glGetUniformLocation(program, "translation");
    glUniform1f(u_scale, game->transform.scale);
    glUniform2f(u_translation, game->transform.position.x, game->transform.position.y);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

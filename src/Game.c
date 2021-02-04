#include "Game.h"

//Super basic vertex and fragment shaders
static const char *vertex_shader = 
"#version 130\n"
"in vec2 pos;\n"
"out vec2 texCoord;\n"
"void main() {\n"
"   gl_Position = vec4(pos, 0, 1);\n"
"   vec2 new_pos = pos * vec2(1.0f, -1.0f);\n"
"   texCoord = (new_pos + 1.0f) / 2.0f;\n"
"}";

static const char *fragment_shader = 
"#version 130\n"
"uniform sampler2D tex;\n"
"in vec2 texCoord;\n"
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
    Game game = {window, width, height};
    return game;
}

//Frees the glfw window
void destroy_game(Game *game) {
    glfwDestroyWindow(game->window);
    game->window = NULL;
}

//Creates opengl shaders and program
void init_opengl_objects() {
    //Create Texture and setup Texture stuff
    GLuint tex = 0;
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
    GLuint program = glCreateProgram();
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

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pos), &pos, GL_STATIC_DRAW);

    GLuint pos_attrib = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos_attrib);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

void draw_texture(uint32_t *tex, int width, int height) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, tex);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

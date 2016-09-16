#include "constants.h"
#include "shader_helper.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>

GLuint vertexArrayID;
GLuint vertexBuffer;
GLuint programID;

static const GLfloat dataArray[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
};

void prepare() {
	glewInit();

    // generate a VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);
    
    // generate a VBO
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(dataArray), dataArray, GL_STATIC_DRAW);

    // initialize shaders
    programID = LoadShaders("white_triangles.vs", "white_triangles.fs");
}

void render() {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        0,
        (void*)0
    );
    glUseProgram(programID);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
}

int main() {
    if (!glfwInit()) {
        return -1;
    }
    
    GLFWwindow* window;

    // initialization for window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "ShadowMap", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    fprintf(stderr, "%s\n",glGetString(GL_VERSION));
    
    
    // prepare
    prepare();
    
    // register function
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;
}
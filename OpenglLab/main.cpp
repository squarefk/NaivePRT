#include "constants.h"
#include "shader_helper.h"
#include "shadow_map_algorithm.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

ShadowMapAlgorithm shadowMapAlgorithm;

void prepare() {
	glewInit();
	shadowMapAlgorithm.prepare();
}

void render() {
	shadowMapAlgorithm.render();
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
		render();
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;
}
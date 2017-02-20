#include "constants.h"
#include "shader_helper.h"
#include "prt_algorithm.h"
#include "shadow_map_algorithm.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ctime>
#include <string>

ShadowMapAlgorithm shadowMapAlgorithm;
PrtAlgorithm prtAlgorithm;

void prepare() {
	glewInit();
//	shadowMapAlgorithm.prepare();
	prtAlgorithm.prepare();
}

void render() {
//	shadowMapAlgorithm.render();
	prtAlgorithm.render();
	Framebuffer::render_to_picture((std::string("test2") + std::to_string(int(time(0)))).c_str());
	exit(0);
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
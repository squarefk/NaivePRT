#include "mesh_builder.h"
#include "constants.h"
#include "framebuffer.h"
#include "shader_helper.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint programID, depthProgramID;
Framebuffer *depthFramebuffer;
glm::mat4 depthMVP;

glm::vec3 LightPosition = glm::vec3(2.5, 5, 2.5);
GLfloat LightPower = 10.f;

MeshBuilder builder;

void prepare() {
	glewInit();

	if (builder.import(std::string(OBJECT_FILE_PATH_PREFIX) +
		std::string("cube.obj"))) {
		fprintf(stderr, "Loading object file successfully.\n");
	}
	builder.generateVAO();
    // initialize shaders
	depthProgramID = LoadShaders("depth_map.vs", "depth_map.fs");
	programID = LoadShaders("shadow_square.vs", "shadow_square.fs");

	depthFramebuffer = new Framebuffer();

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void render_depth_map() {
	depthFramebuffer->render();
//	Framebuffer::render_to_screen();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 Projection = glm::ortho<float>(-5.f, 5.f, -5.f, 5.f, 0.f, 10.f);
	glm::mat4 View = glm::lookAt(
		LightPosition,		// Position of camera, in World Space
		glm::vec3(0, -1, 0),	// and looks at the origin
		glm::vec3(0, 1, 0)	// Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	
	depthMVP = Projection * View * Model;

	glUseProgram(depthProgramID);
	GLuint depthMVPID = glGetUniformLocation(depthProgramID, "depthMVP");
	glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);

	builder.draw0();
}

void render_scene() {
	Framebuffer::render_to_screen();
//	depthFramebuffer->render();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 1.f, 0.1f, 400.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(-3, 3, -3),	// Position of camera, in World Space
		glm::vec3(0, -1, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glUseProgram(programID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthFramebuffer->get_texture_id());
	GLuint depthTextureID = glGetUniformLocation(programID, "depthTexture");
	glUniform1i(depthTextureID, 0);
	GLuint depthMVPID = glGetUniformLocation(programID, "depthMVP");
	glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);
	GLuint MVPID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	GLuint ModelID = glGetUniformLocation(programID, "M");
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);
	GLuint ViewID = glGetUniformLocation(programID, "V");
	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);
	GLuint LightPositionID = glGetUniformLocation(programID, "LightPosition");
	glUniform3fv(LightPositionID, 1, &LightPosition[0]);
	GLuint LightPowerID = glGetUniformLocation(programID, "LightPower");
	glUniform1f(LightPowerID, LightPower);

	builder.draw();
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
		render_depth_map();
		render_scene();
		glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwTerminate();
    
    return 0;
}
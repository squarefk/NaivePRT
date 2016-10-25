#include "mesh_builder.h"
#include "constants.h"
#include "framebuffer.h"
#include "shader_helper.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLuint vertexArrayID;
GLuint vertexBufferPosition;
GLuint vertexBufferColor;
GLuint vertexBufferNormal;
GLuint programID, depthProgramID;

static const GLfloat positionData[] = {
	-10.0f, 0.0f, -10.0f,
	10.0f, 0.0f, -10.0f,
	-10.0f, 0.0f, 10.0f,
	-10.0f, 0.0f, 10.0f,
	10.0f, 0.0f, -10.0f,
	10.0f, 0.0f, 10.0f,

	-4.0f, 2.0f, -4.0f,
	4.0f, 2.0f, -4.0f,
	-4.0f, 2.0f, 4.0f,
	-4.0f, 2.0f, 4.0f,
	4.0f, 2.0f, -4.0f,
	4.0f, 2.0f, 8.0f

};

static const GLfloat colorData[] = {
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,

	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f
};

static const GLfloat normalData[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

Framebuffer *depthFramebuffer;
glm::mat4 depthMVP;

glm::vec3 LightPosition = glm::vec3(0, 15, 0);
GLfloat LightPower = 200.f;

void prepare() {
	glewInit();

    // generate a VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

	// generate a VBO for position
	glGenBuffers(1, &vertexBufferPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// generate a VBO for color
	glGenBuffers(1, &vertexBufferColor);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// generate a VBO for normal
	glGenBuffers(1, &vertexBufferNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalData), normalData, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindVertexArray(0);

    // initialize shaders
	depthProgramID = LoadShaders("depth_map.vs", "depth_map.fs");
	programID = LoadShaders("shadow_square.vs", "shadow_square.fs");

	depthFramebuffer = new Framebuffer();
}

void render_depth_map() {
	depthFramebuffer->render();
//	Framebuffer::render_to_screen();
	glClear(GL_COLOR_BUFFER_BIT);
	
	glm::mat4 Projection = glm::ortho<float>(-15, 15, -15, 15, -15, 15);
	glm::mat4 View = glm::lookAt(
		LightPosition, // Position of camera, in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 0, 1)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);

	depthMVP = Projection * View * Model;

	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);

	glUseProgram(depthProgramID);
	GLuint depthMVPID = glGetUniformLocation(depthProgramID, "depthMVP");
	glUniformMatrix4fv(depthMVPID, 1, GL_FALSE, &depthMVP[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void render_scene() {
	Framebuffer::render_to_screen();
//	depthFramebuffer->render();
	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 8, 20),	// Position of camera, in World Space
		glm::vec3(0, 0, 0),		// and looks at the origin
		glm::vec3(0, 1, 0)		// Head is up (set to 0,-1,0 to look upside-down)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

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

	glDrawArrays(GL_TRIANGLES, 0, 12);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}

int main() {
	MeshBuilder builder;
	if (builder.import(std::string(OBJECT_FILE_PATH_PREFIX) + 
		std::string("cube.obj"))) {
		fprintf(stderr, "Loading object file successfully.");
	}

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
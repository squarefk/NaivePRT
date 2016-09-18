#include "constants.h"
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
GLuint programID;

static const GLfloat positionData[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

static const GLfloat colorData[] = {
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f,
	0.66f, 0.33f, 0.33f
};

static const GLfloat normalData[] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
};



void prepare() {
	glewInit();

    // generate a VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

	// generate a VBO for position
	glGenBuffers(1, &vertexBufferPosition);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPosition);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positionData), positionData, GL_STATIC_DRAW);

	// generate a VBO for color
	glGenBuffers(1, &vertexBufferColor);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);

	// generate a VBO for normal
	glGenBuffers(1, &vertexBufferNormal);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNormal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normalData), normalData, GL_STATIC_DRAW);

    // initialize shaders
    programID = LoadShaders("white_triangles.vs", "white_triangles.fs");
}

void render() {
	// Projection matrix : 45бу Field of View, 4:3 ratio, display range : 0.1 unit  100 units
	glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);  // Changes for each model !
										// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	glm::vec3 LightPosition = glm::vec3(1, 1, 1);

	GLuint MVPID = glGetUniformLocation(programID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);

	GLuint ModelID = glGetUniformLocation(programID, "M");
	glUniformMatrix4fv(ModelID, 1, GL_FALSE, &Model[0][0]);

	GLuint ViewID = glGetUniformLocation(programID, "V");
	glUniformMatrix4fv(ViewID, 1, GL_FALSE, &View[0][0]);

	GLuint LightPositionID = glGetUniformLocation(programID, "LightPosition");
	glUniformMatrix4fv(LightPositionID, 1, GL_FALSE, &LightPosition[0]);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPosition);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColor);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNormal);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

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
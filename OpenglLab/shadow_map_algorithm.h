#ifndef SHADOW_MAP_ALGORITHM_H
#define SHADOW_MAP_ALGORITHM_H

#include "framebuffer.h"
#include "mesh_builder.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ShadowMapAlgorithm {
	Framebuffer *depthFramebuffer;
	GLuint programID, depthProgramID;
	glm::mat4 depthMVP;
	glm::vec3 LightPosition = glm::vec3(2.5, 5, 2.5);
	GLfloat LightPower = 10.f;
	MeshBuilder builder;

	void render_shadow_map();
	void render_target();

public:
	void prepare();
	void render();
};

#endif
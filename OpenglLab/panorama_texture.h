#ifndef PANORAMA_TEXTURE_H
#define PANORAMA_TEXTURE_H

#include "texture.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PanoramaTexture {
	Texture* texture;
	GLuint textureID;

public:
	PanoramaTexture();
	~PanoramaTexture();
	void load(const char* filePath);
	GLuint get_texture_id();
	glm::vec3 get_color(float phi, float theta);
};

#endif
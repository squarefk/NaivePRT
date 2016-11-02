#ifndef TEXTURE_H
#define TEXTURE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <map>
#include <vector>
#include <string>
#include <utility>

class Texture {
	int width;
	int height;
	int channels;
	unsigned char* data;
public:
	Texture(std::string fileName);
	glm::vec3 get_color(float u, float v);
	unsigned char* get_pointer();
	int get_width();
	int get_height();
	int get_channels();
};

#endif
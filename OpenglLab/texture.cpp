#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <algorithm>

using namespace std;

Texture::Texture(string fileName) {
	fprintf(stderr, "Start loading image: %s\n", fileName.c_str());
	data = stbi_load(fileName.c_str(), &width, &height, &channels, 0);
	if (data == NULL) {
		fprintf(stderr, "Occur error when loading texture: %s\n", fileName.c_str());
	} else {
		fprintf(stderr, "Finish loading image %s successfully\n", fileName.c_str());
	}
}

glm::vec3 Texture::get_color(float u, float v) {
	u = u - floor(u);
	v = v - floor(v);
	if (u < 0 || u > 1 || v < 0 || v > 1) {
		fprintf(stderr, "Error uv address : %.2f %.2f\n", u, v);
		return glm::vec3(0, 0, 0);
	}
	int w = int(u * (width-1));
	int h = int(v * (height-1));
	float r = float(data[(h * width + w) * channels]) / 255.f;
	float g = float(data[(h * width + w) * channels + 1]) / 255.f;
	float b = float(data[(h * width + w) * channels + 2]) / 255.f;
	return glm::vec3(r, g, b);
}

unsigned char* Texture::get_pointer() {
	return data;
}

int Texture::get_width() {
	return width;
}

int Texture::get_height() {
	return height;
}

int Texture::get_channels() {
	return channels;
}

#undef STB_IMAGE_IMPLEMENTATION

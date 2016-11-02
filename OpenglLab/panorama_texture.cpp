#include "panorama_texture.h"

#include "constants.h"

PanoramaTexture::PanoramaTexture() {

}

PanoramaTexture::~PanoramaTexture() {
	delete texture;
}

void PanoramaTexture::load(const char* fileName) {
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	texture = new Texture((std::string(IMAGE_FILE_PATH_PREFIX) + std::string(fileName)).c_str());
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB,
		texture->get_width(), texture->get_height(), 0,
		GL_BGR, GL_UNSIGNED_BYTE, texture->get_pointer()
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint PanoramaTexture::get_texture_id() {
	return textureID;
}

glm::vec3 PanoramaTexture::get_color(float phi, float theta) {
	return texture->get_color(phi / 2.f / M_PI, theta / M_PI);
}
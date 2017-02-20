#define _CRT_SECURE_NO_WARNINGS

#include "framebuffer.h"

#include "constants.h"

#include <cstdio>
#include <string>

Framebuffer::Framebuffer(int width, int height) {
	this->width = width;
	this->height = height;

	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	glGenTextures(1, &TextureName);
	glBindTexture(GL_TEXTURE_2D, TextureName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &DepthName);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthName);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthName);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, TextureName, 0);
	GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Something goes wrong when creating Framebuffer object.\n");
	}
}

Framebuffer::~Framebuffer() {
	glDeleteFramebuffers(1, &FramebufferName);
}


void Framebuffer::render() {
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glViewport(0, 0, width, height);
}

void Framebuffer::render_to_screen() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
unsigned char Framebuffer::pictureData[WINDOW_HEIGHT][WINDOW_WIDTH][3];
void Framebuffer::render_to_picture(const char* pictureName) {
	GLint viewPort[4] = { 0 };
	glGetIntegerv(GL_VIEWPORT, viewPort);
	glReadPixels(viewPort[0], viewPort[1], viewPort[2], viewPort[3], GL_RGB, GL_UNSIGNED_BYTE, pictureData);
	for (int h = 0; h < WINDOW_HEIGHT / 2; ++h)
		for (int w = 0; w < WINDOW_WIDTH; ++w)
			for (int i = 0; i < 3; ++i)
				std::swap(pictureData[h][w][i], pictureData[WINDOW_HEIGHT - 1 - h][w][i]);
	std::string picturePath = std::string(SCREENSHOT_FILE_PATH_PREFIX) + std::string(pictureName) + std::string(".png");
	if (!stbi_write_png(picturePath.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, 3, pictureData, WINDOW_WIDTH * 3)) {
		fprintf(stderr, "Occur error when outputing to picture\n");
	}
}
#undef STB_IMAGE_IMPLEMENTATION

GLuint Framebuffer::get_texture_id() {
	return TextureName;
}

#undef _CRT_SECURE_NO_WARNINGS

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define  GLEW_STATIC
#include "constants.h"
#include <GL/glew.h>

class Framebuffer{
public:
	Framebuffer(int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT);
	~Framebuffer();
	void render();
	GLuint get_texture_id();
	static void render_to_screen();
	static unsigned char pictureData[WINDOW_HEIGHT][WINDOW_WIDTH][3];
	static void render_to_picture(const char* pictureName = "output");

private:
	int width;
	int height;
	GLuint FramebufferName;
	GLuint TextureName;
	GLuint DepthName;
};

#endif

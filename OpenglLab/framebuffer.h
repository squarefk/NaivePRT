#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#define  GLEW_STATIC
#include <GL/glew.h>

class Framebuffer{
public:
	Framebuffer(int width = 1024, int height = 1024);
	~Framebuffer();
	void render();
	static void render_to_screen();

private:
	int width;
	int height;
	GLuint FramebufferName;
	GLuint TextureName;
};

#endif
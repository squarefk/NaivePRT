#ifndef SHADER_HELPER_H
#define SHADER_HELPER_H

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>

class ShaderHelper {

public:
	static GLuint LoadShaders(const char * vertex_file_name, const char * fragment_file_name);
};

#endif /* shader_helper_h */

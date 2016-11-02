#version 330 core

in vec3 ColorPassed;
out vec3 color;

void main() {
	color = ColorPassed;
}
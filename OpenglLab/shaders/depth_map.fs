#version 330 core

out vec3 color;

void main() {
	float depth = gl_FragCoord.z;
	color = vec3(depth, depth, depth);
}
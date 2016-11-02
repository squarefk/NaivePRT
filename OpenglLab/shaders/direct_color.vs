#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
out vec3 ColorPassed;

uniform mat4 MVP;

void main(){
	gl_Position = MVP * vec4(vertexPosition, 1);
	ColorPassed = vertexColor;
}
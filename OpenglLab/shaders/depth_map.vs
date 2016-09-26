#version 330 core

in vec3 vertexPosition;
uniform mat4 depthMVP;

void main(){
	gl_Position = depthMVP * vec4(vertexPosition, 1);
}
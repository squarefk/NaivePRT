#version 330 core

in vec3 vertexPosition;
out vec3 angle;

uniform mat4 rotate_matrix;

void main(){
	gl_Position = vec4(-vertexPosition.x, vertexPosition.y, 1, 1);
	angle = vertexPosition + vec3(0,0,1);
	angle = (vec4(angle, 0) * rotate_matrix).xyz;
}
#version 330 core

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
out vec3 Normal_cameraspace;
out vec3 LightDirection_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 MaterialDiffuseColor;
out vec3 VertexPositionPassed;
out vec4 ShadowCoordPassed;
uniform mat4 depthMVP;
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 LightPosition;

void main(){
	gl_Position = MVP * vec4(vertexPosition, 1);
	ShadowCoordPassed = depthMVP * vec4(vertexPosition, 1);

	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition, 1)).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;
	vec3 LightPosition_cameraspace = (V * vec4(LightPosition, 1)).xyz;
	LightDirection_cameraspace = LightPosition + EyeDirection_cameraspace;
	Normal_cameraspace = (V * M * vec4(vertexNormal, 0)).xyz;

	MaterialDiffuseColor = vertexColor;
	VertexPositionPassed = (M * vec4(vertexPosition, 1)).xyz;

}
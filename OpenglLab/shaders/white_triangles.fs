#version 330 core

in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 MaterialDiffuseColor;
in vec3 VertexPositionPassed;
out vec3 color;

uniform vec3 LightPosition;
uniform float LightPower;

void main() {
	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);
	float cosTheta = clamp(dot(n, l), 0, 1);
	vec3 MaterialAmbientColor = MaterialDiffuseColor * vec3(0.1, 0.1, 0.1);

	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l, n);
	float cosAlpha = clamp(dot(E, R), 0, 1);

//	color =
//		MaterialAmbientColor +
//		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
//		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha, 5) / (distance * distance);

	vec3 delta = LightPosition - VertexPositionPassed;
	float DistanceDistance = dot(delta, delta);

	color =
		MaterialAmbientColor +
		MaterialDiffuseColor * cosTheta * LightPower / DistanceDistance;
}
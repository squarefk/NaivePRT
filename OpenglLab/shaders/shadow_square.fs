#version 330 core

in vec3 Normal_cameraspace;
in vec3 LightDirection_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 MaterialDiffuseColor;
in vec3 VertexPositionPassed;
in vec4 ShadowCoordPassed;
out vec3 color;

uniform vec3 LightPosition;
uniform float LightPower;
uniform sampler2D depthTexture;

void main() {
	vec3 n = normalize(Normal_cameraspace);
	vec3 l = normalize(LightDirection_cameraspace);
	float cosTheta = clamp(dot(n, l), 0, 1);
	vec3 MaterialAmbientColor = MaterialDiffuseColor * vec3(0.1, 0.1, 0.1);

//	vec3 E = normalize(EyeDirection_cameraspace);
//	vec3 R = reflect(-l, n);
//	float cosAlpha = clamp(dot(E, R), 0, 1);

	vec3 delta = LightPosition - VertexPositionPassed;
	float DistanceDistance = dot(delta, delta);

	float bias = 0.005;
	float visibility = 1.0;

	vec3 ShadowCoord = ShadowCoordPassed.xyz / ShadowCoordPassed.w;
	ShadowCoord = (ShadowCoord + vec3(1, 1, 1)) * 0.5;
	if (texture2D(depthTexture, ShadowCoord.xy).x < ShadowCoord.z - bias) {
		visibility = 0.5;
	}

	color =
		MaterialAmbientColor +
		visibility * MaterialDiffuseColor * cosTheta * LightPower / DistanceDistance;
}
#include "prt_algorithm.h"

#include "constants.h"
#include "texture.h"
#include "shader_helper.h"

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

void PrtAlgorithm::generate_sample_angles() {
	int sqrtSamps = (int)sqrtf(1.f * samps);
	if (sqrtSamps * sqrtSamps != samps) {
		fprintf(stderr, "Error! The number of samples must be perfect square.");
	}
	for (int i = 0; i < sqrtSamps; ++i)
		for (int j = 0; j < sqrtSamps; ++j) {
			float x = (1.f * rand() / RAND_MAX + 1.f * i) / float(sqrtSamps);
			float y = (1.f * rand() / RAND_MAX + 1.f * j) / float(sqrtSamps);
			theta.push_back(2.f * acos(sqrt(1.f - x)));
			phi.push_back(y * 2.f * M_PI);
		}
}

int PrtAlgorithm::p(int l,int m) {
	return l * l + l + m;
}

void PrtAlgorithm::calc_sh_coeff(float theta_, float phi_, std::vector<float>& y_coeff) {
	for (int j = 0; j < (lmax + 1) * (lmax + 1); ++j) {
		y_coeff.push_back(0.f);
	}
	float x = cos(theta_);

	// step 1
	p_coeff[p(0, 0)] = 1.f;
	p_coeff[p(1, 0)] = x;
	for (int l = 2; l <= lmax; ++l)
		p_coeff[p(l, 0)] =
			((2.f * l - 1) * x * p_coeff[p(l - 1, 0)] -
			(1.f * l - 1) * p_coeff[p(l - 2, 0)]) / float(l);
	// step 2
	float neg = -1.f;
	float dfact = 1.f;
	float xroot = sqrtf(std::max(0.f, 1.f - x * x));
	float xpow = xroot;
	for (int l = 1; l <= lmax; ++l) {
		p_coeff[p(l, l)] = neg * dfact * xpow;
		neg *= -1.f;
		dfact *= 2.f * l + 1.f;
		xpow *= xroot;
	}
	// step 3
	for (int l = 2; l <= lmax; ++l)
		p_coeff[p(l, l - 1)] =
			x * (2.f * l - 1.f) * p_coeff[p(l - 1, l - 1)];
	// step 4
	for (int l = 3; l <= lmax; ++l)
		for (int m = 1; m <= l - 2; ++m)
			p_coeff[p(l, m)] =
				((2.f * (l - 1.f) + 1.f) * x * p_coeff[p(l - 1, m)] -
				(1.f * l - 1.f + m) * p_coeff[p(l - 2, m)]) / (l - m);
	// step 5
	for (int l = 0; l <= lmax; ++l)
		for (int m = -l; m <= l; ++m) {
			float k = (2.f * l + 1.f) / 4.f / M_PI;
			for (int i = l - abs(m) + 1; i <= l + abs(m); ++i)
				k /= float(i);
			k = sqrtf(k);
			k_coeff[p(l, m)] = k;
		}
	// step 6
	sin_coeff[0] = 0;
	cos_coeff[0] = 1;
	sin_coeff[1] = sin(phi_);
	cos_coeff[1] = cos(phi_);
	for (int l = 2; l <= lmax; ++l) {
		sin_coeff[l] = sin_coeff[l - 1] * cos_coeff[1] + cos_coeff[l - 1] * sin_coeff[1];
		cos_coeff[l] = cos_coeff[l - 1] * cos_coeff[1] - sin_coeff[l - 1] * sin_coeff[1];
	}
	// step 7
	for (int l = 0; l <= lmax; ++l) {
		for (int m = -l; m < 0; ++m) {
			y_coeff[p(l, m)] = sqrtf(2.f) * k_coeff[p(l, m)] * p_coeff[p(l, -m)] * sin_coeff[-m];
		}
		y_coeff[p(l, 0)] = k_coeff[p(l, 0)] * p_coeff[p(l, 0)];
		for (int m = 1; m <= l; ++m) {
			y_coeff[p(l, m)] = sqrtf(2.f) * k_coeff[p(l, m)] * p_coeff[p(l, m)] * cos_coeff[m];
		}
	}
}

glm::vec3 PrtAlgorithm::precomputed_diffuse_color(int i) {
	int lmaxlmax = (lmax + 1) * (lmax + 1);
	for (int j = 0; j < lmaxlmax; ++j) {
		light_coeff[j] = glm::vec3(0, 0, 0);
		transfer_coeff[j] = glm::vec3(0, 0, 0);
	}
	for (int j = 0; j < samps; ++j) {
		glm::vec3 color_value = panorama.get_color(phi[j], theta[j]);
		// calc light
		for (int k = 0; k < lmaxlmax; ++k) {
			light_coeff[k] += color_value * y_coeff[j][k];
		}
		// calc transfer
		glm::vec3 pos(model.positionData[i], model.positionData[i + 1], model.positionData[i + 2]);
		glm::vec3 dir(sin(theta[j]) * cos(phi[j]), sin(theta[j]) * sin(phi[j]), cos(theta[j]));
		if (!bvhTree.ray_intersect_with_mesh(BvhTree::Ray(pos, dir))) {
			float cos_value = glm::dot(
				glm::vec3(model.normalData[i], model.normalData[i + 1], model.normalData[i + 2]),
				glm::vec3(sin(theta[j]) * cos(phi[j]), sin(theta[j]) * sin(phi[j]), cos(theta[j]))
			);
			for (int k = 0; k < lmaxlmax; ++k) {
				float single = std::max(cos_value, 0.f) * y_coeff[j][k];
				transfer_coeff[k] += glm::vec3(single, single, single);
			}
		}
	}
	glm::vec3 color(0, 0, 0);
	for (int j = 0; j < lmaxlmax; ++j) {
		light_coeff[j] *= (4.f * M_PI) / float(samps);
		transfer_coeff[j] *= (4.f * M_PI) / float(samps);
		color += light_coeff[j] * transfer_coeff[j] / M_PI;
	}
	return color;
}

// Bank BRDF
float PrtAlgorithm::brdf(int inIndex, int outIndex) {
	glm::vec3 N{ 0, 0, 1 };
	glm::vec3 L{ sin(theta[inIndex]) * cos(phi[inIndex]), sin(theta[inIndex]) * sin(phi[inIndex]), cos(theta[inIndex]) };
	glm::vec3 V{ sin(theta[outIndex]) * cos(phi[outIndex]), sin(theta[outIndex]) * sin(phi[outIndex]), cos(theta[outIndex]) };
	glm::vec3 T = glm::cross(N, V);
	if (L.z < 0 || V.z < 0) {
		return 0.f;
	}
	float LT = glm::dot(L, T);
	float VT = glm::dot(V, T);
	return 0.1f + 0.9f * pow(sqrt(1.f - LT * LT) * sqrt(1.f - VT * VT) - LT * VT, 2);
}

float PrtAlgorithm::brdf(int inIndex, glm::vec3 V) {
	glm::vec3 N{ 0, 0, 1 };
	glm::vec3 L{ sin(theta[inIndex]) * cos(phi[inIndex]), sin(theta[inIndex]) * sin(phi[inIndex]), cos(theta[inIndex]) };
	glm::vec3 T = glm::cross(N, V);
	if (L.z < 0 || V.z < 0) {
		return 0.f;
	}
	float LT = glm::dot(L, T);
	float VT = glm::dot(V, T);
	return 0.1f + 0.9f * pow(sqrt(1.f - LT * LT) * sqrt(1.f - VT * VT) - LT * VT, 2);
}

void toZYZ(const glm::mat3 &m, float *alpha, float *beta, float *gamma) {
#define M(a, b) (m[a][b])

	float sy = sqrtf(M(2, 1)*M(2, 1) + M(2, 0)*M(2, 0));
	if (sy > 16 * FLT_EPSILON) {
		*gamma = -atan2f(M(1, 2), -M(0, 2));
		*beta = -atan2f(sy, M(2, 2));
		*alpha = -atan2f(M(2, 1), M(2, 0));
	}
	else {
		*gamma = 0;
		*beta = -atan2f(sy, M(2, 2));
		*alpha = -atan2f(-M(1, 0), M(1, 1));
	}
#undef M
}

glm::vec3 PrtAlgorithm::precomputed_bsdf_color(int i) {
	std::vector<glm::vec3> light, shadow, local, reflect;

	int lmaxlmax = (lmax + 1) * (lmax + 1);

	// calc vector (light)
	for (int j = 0; j < lmaxlmax; ++j) {
		glm::vec3 light_coeff{ 0, 0, 0 };
		for (int k = 0; k < samps; ++k) {
			glm::vec3 color_value = panorama.get_color(phi[k], theta[k]);
			light_coeff += color_value * y_coeff[k][j];
		}
		light.push_back(light_coeff * (4.f * M_PI) / float(samps));
	}

	// clac T * vector (shadow)
	for (int j = 0; j < lmaxlmax; ++j)
		shadow.push_back(glm::vec3(0, 0, 0));
	for (int k = 0; k < samps; ++k) {
		glm::vec3 pos(model.positionData[i], model.positionData[i + 1], model.positionData[i + 2]);
		glm::vec3 dir(sin(theta[k]) * cos(phi[k]), sin(theta[k]) * sin(phi[k]), cos(theta[k]));
		if (!bvhTree.ray_intersect_with_mesh(BvhTree::Ray(pos, dir))) {
			for (int j = 0; j < lmaxlmax; ++j)
				for (int p = 0; p < lmaxlmax; ++p)
					shadow[j] += light[p] * y_coeff[k][j] * y_coeff[k][p];
		}
	}
	for (int j = 0; j < lmaxlmax; ++j)
		shadow[j] *= (4.f * M_PI) / float(samps);

	// calc R * T * vector (local)
	glm::vec3 zcz = glm::cross(glm::vec3(0, 0, 1), glm::vec3(model.normalData[i], model.normalData[i + 1], model.normalData[i + 2]));
	float zdz = glm::dot(glm::vec3(0, 0, 1), glm::vec3(model.normalData[i], model.normalData[i + 1], model.normalData[i + 2]));
	float alpha = atan2(zcz.y, zcz.x) - M_PI * 0.5f;
	float beta = acos(zdz);
	float gamma = 0;
	glm::mat3 m1{ cos(alpha),sin(alpha),0,-sin(alpha),cos(alpha),0,0,0,1 };
	glm::mat3 m2{ cos(beta),0,-sin(beta),0,1,0,sin(beta),0,cos(beta) };
	glm::mat3 m3{ cos(gamma),sin(gamma),0,-sin(gamma),cos(gamma),0,0,0,1 };
//	glm::mat3 m4{ 1, 0, 0, 0, cos(M_PI/2.f),-sin(M_PI / 2.f),0,sin(M_PI / 2.f),cos(M_PI / 2.f) };
	glm::mat3 rotate = m1 * m2 * m3;
//	fprintf(stderr, "before %.4f %.4f %.4f\n", alpha, beta, gamma);
	toZYZ(rotate, &alpha, &beta, &gamma);
	glm::mat3 mm1{ cos(gamma),sin(gamma),0,-sin(gamma),cos(gamma),0,0,0,1 };
	glm::mat3 mm2{ 1, 0, 0, 0, cos(M_PI*0.5f), sin(M_PI*0.5f) ,0, -sin(M_PI*0.5f), cos(M_PI*0.5f) };
	glm::mat3 mm3{ cos(beta),sin(beta),0,-sin(beta),cos(beta),0,0,0,1 };
	glm::mat3 mm4{ 1, 0, 0, 0, cos(-M_PI*0.5f), sin(-M_PI*0.5f) ,0, -sin(-M_PI*0.5f), cos(-M_PI*0.5f) };
	glm::mat3 mm5{ cos(beta),sin(beta),0,-sin(beta),cos(beta),0,0,0,1 };
	/*
	fprintf(stderr, "\n===========\n");
	for (int j = 0; j < 3; ++j) {
		for (int k = 0; k < 3; ++k)
			fprintf(stderr, "%.2f ", rotate[j][k]); fprintf(stderr, "\n");
	}
	for (int j = 0; j < 3; ++j) {
		for (int k = 0; k < 3; ++k)
			fprintf(stderr, "%.2f ", (mm5*mm4*mm3*mm2*mm1)[j][k]); fprintf(stderr, "\n");
	}
	fprintf(stderr, "\n===========\n");
	*/
	//	fprintf(stderr,"after %.4f %.4f %.4f\n", alpha,beta,gamma);
//	glm::vec3 n;
//	n = rotate * glm::vec3(0, 0, 1) - glm::vec3(model.normalData[i], model.normalData[i + 1], model.normalData[i + 2]);
//	fprintf(stderr, "==%.4f %.4f %.4f\n", n.x, n.y, n.z);
	std::vector<glm::vec3> tmp1, tmp2, tmp3, tmp4;
	rotate_z(shadow, tmp1, -gamma);
	rotate_x_minus(tmp1, tmp2);
	rotate_z(tmp2, tmp3, -beta);
	rotate_x_plus(tmp3, tmp4);
	rotate_z(tmp4, local, -beta);

	// calc B * R * T * vector (reflection)
	/*
		for (int j = 0; j < lmaxlmax; ++j) {
			glm::vec3 reflect_coeff = glm::vec3(0,0,0);
			for (int k = 0; k < lmaxlmax; ++k)
				for (int p = 0; p < samps; ++p)
					for (int q = 0; q < samps; ++q)
						reflect_coeff += local[k] * abs(cos(theta[p])) * y_coeff[p][k] * y_coeff[q][j] * brdf(p, q);
			reflect.push_back(reflect_coeff * (4.f * M_PI) / float(samps) * (4.f * M_PI) / float(samps));
		}

		// calc one point color
		glm::vec3 dir = glm::normalize(get_camera_position() - glm::vec3(model.positionData[i], model.positionData[i+1], model.positionData[i+2]));
		dir = rotate * dir;
		std::vector<float> yy;
		calc_sh_coeff(acos(dir.z),atan2(dir.y, dir.x),yy);
		glm::vec3 color{0, 0, 0};
		for (int j = 0; j < lmaxlmax; ++j)
			color += yy[j] * reflect[j] / M_PI;
	*/

	//	glm::vec3 dir = glm::normalize(get_camera_position() - glm::vec3(model.positionData[i], model.positionData[i + 1], model.positionData[i + 2]));
	//	dir = rotate * dir;
	glm::vec3 color{ 0,0,0 };
	for (int j = 0; j < lmaxlmax; ++j)
		for (int p = 0; p < samps; ++p) {
			glm::vec3 n1{ model.normalData[i],model.normalData[i + 1], model.normalData[i + 2] };
			glm::vec3 v1{sin(theta[p]) * cos(phi[p]), sin(theta[p]) * sin(phi[p]), cos(theta[p])};
			if (glm::dot(mm5 * mm4 * mm3 * mm2 * mm1 * v1, n1) > 0)
//			if (cos(theta[p]) > 0)
//			if (glm::dot(n1,v1) > 0)
				color += local[j] * y_coeff[p][j] / 2.f;// *cos(theta[p]) * brdf(p, dir);
//				color += shadow[j] * y_coeff[p][j] / 2.f;// *std::max(0.f, glm::dot(n1, v1));// *brdf(p, dir);
		}
	// yi ding yao shan aaaaa /2.
	
	color *= (4.f * M_PI) / float(samps);
	// fprintf(stderr, "%.4f\n", color.x);

	return color / M_PI / 2.f;
}

void PrtAlgorithm::update_rotate_angle() {
	rotate_angle += 0.01f;
}

glm::vec3 PrtAlgorithm::get_camera_position() {
	return glm::vec3(
		cos(M_PI * 1.5f + rotate_angle) * distance * 2,
		0,
		sin(M_PI * 1.5f + rotate_angle) * distance * 2
	);
}

glm::vec3 PrtAlgorithm::get_camera_focus() {
	return glm::vec3(0, 0, 0);
}

void PrtAlgorithm::prepare() {
	fprintf(stderr, "Prt: precomputed processed starts.\n");
	generate_sample_angles();

	for (int i = 0; i < (lmax + 1) * (lmax + 1); ++i) {
		p_coeff.push_back(0.f);
		k_coeff.push_back(0.f);
		cos_coeff.push_back(0.f);
		sin_coeff.push_back(0.f);
		light_coeff.push_back(glm::vec3());
		transfer_coeff.push_back(glm::vec3());
	}
	for (int i = 0; i < samps; ++i) {
		calc_sh_coeff(theta[i],phi[i],y_coeff[i]);
	}

	panorama.load("white.jpg");
	curtain.import(std::string(""), 1);
	curtain.generateVAO();
	distance = model.import(std::string("sphere.obj"), -1);
	rotate_angle = M_PI;
	bvhTree.load(model);

	time_t startTime,currentTime;
	startTime = time(NULL);
	for (unsigned int i = 0; i < model.positionData.size(); i += 3) {
		currentTime = time(NULL);
		fprintf(stderr, "Process model in %.2f%%, remain %.2f s         \r",
			100.f * (i + 1) / model.positionData.size(),
			float(currentTime - startTime) / (i + 1) * (model.positionData.size() - i));
//		glm::vec3 color = precomputed_diffuse_color(i)/ 2.f;
		glm::vec3 color = precomputed_bsdf_color(i);
		model.colorData[i] = color.r;
		model.colorData[i+1] = color.g;
		model.colorData[i+2] = color.b;
	}

	model.generateVAO();

	modelProgramID = ShaderHelper::LoadShaders("direct_color.vs", "direct_color.fs");
	curtainProgramID = ShaderHelper::LoadShaders("cubemap.vs", "cubemap.fs");

	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	fprintf(stderr, "\nPrt: precomputed processed finishes.\n");
}

void PrtAlgorithm::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	update_rotate_angle();
	glm::mat4 Projection = glm::perspective(glm::radians(90.0f), 1.f, 0.1f, 400.0f);
	glm::mat4 View = glm::lookAt(
		get_camera_position(),
		get_camera_focus(),
		glm::vec3(0, 1, 0)
	);
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glUseProgram(modelProgramID);
	GLuint MVPID = glGetUniformLocation(modelProgramID, "MVP");
	glUniformMatrix4fv(MVPID, 1, GL_FALSE, &MVP[0][0]);
	model.draw();

	glm::mat4 rotate_matrix;
	rotate_matrix[0][0] = cosf(rotate_angle);
	rotate_matrix[0][2] = -sinf(rotate_angle);
	rotate_matrix[2][0] = sinf(rotate_angle);
	rotate_matrix[2][2] = cosf(rotate_angle);
	rotate_matrix[1][1] = rotate_matrix[3][3] = 1;
	
	glUseProgram(curtainProgramID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, panorama.get_texture_id());
	GLuint panoramaID = glGetUniformLocation(curtainProgramID, "panorama");
	glUniform1i(panoramaID, 0);
	GLuint rotateMatrixID = glGetUniformLocation(curtainProgramID, "rotate_matrix");
	glUniformMatrix4fv(rotateMatrixID, 1, GL_FALSE, &rotate_matrix[0][0]);

	curtain.draw0();
}

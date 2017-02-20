#ifndef PRT_ALGORITHM_H
#define PRT_ALGORITHM_H

#include "panorama_texture.h"
#include "mesh_builder.h"
#include "bvh_tree.h"

#include <vector>

class PrtAlgorithm {
	int lmax = 5;
	const static int samps = 10000;

	std::vector<float> phi, theta;
	std::vector<float> p_coeff;
	std::vector<float> k_coeff;
	std::vector<float> y_coeff[samps];
	std::vector<float> cos_coeff;
	std::vector<float> sin_coeff;
	std::vector<glm::vec3> light_coeff, transfer_coeff;

	float distance;
	float rotate_angle;

	BvhTree bvhTree;

	void update_rotate_angle();
	glm::vec3 get_camera_position();
	glm::vec3 get_camera_focus();
	GLuint modelProgramID, curtainProgramID;
	MeshBuilder model, curtain;
	PanoramaTexture panorama;

	int p(int l, int m);
	glm::vec3 precomputed_diffuse_color(int i);
	glm::vec3 precomputed_bsdf_color(int i);
	float brdf(int p, int q);
	float brdf(int p, glm::vec3);
	void rotate_matrix_to_zyz(const glm::mat3 &m, float *alpha, float *beta, float *gamma);
	void rotate_x_plus(const std::vector<glm::vec3>& vin, std::vector<glm::vec3>& vout);
	void rotate_x_minus(const std::vector<glm::vec3>& vin, std::vector<glm::vec3>& vout);
	void rotate_z(const std::vector<glm::vec3>& vin, std::vector<glm::vec3>& vout, float angle);
	void generate_sample_angles();
	void calc_sh_coeff(float theta_, float phi_, std::vector<float>& y_coeff);
public:
	void prepare();
	void render();
};


#endif
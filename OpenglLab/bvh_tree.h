#ifndef BVH_TREE_H
#define BVH_TREE_H

#include "mesh_builder.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BvhTree {
public:
	struct Triangle {
		Triangle(glm::vec3 _p, glm::vec3 _q, glm::vec3 _r);
		glm::vec3 p, q, r;
	};
	struct Ray {
		Ray(glm::vec3 _pos, glm::vec3 _dir);
		glm::vec3 pos, dir, inv;
	};
	struct Node {
		int l, r, m;
		glm::vec3 low;
		glm::vec3 high;
	};
private:
	float dist;
	int* queue;
	Node* t;
	std::vector<Triangle> triangles;
	void build(int k, int l, int r);
	bool ray_intersect_with_triangle(const Ray& ray, const Triangle& tri);
	bool ray_intersect_with_box(const Ray& ray, const glm::vec3& low, const glm::vec3& high);
public:
	BvhTree();
	~BvhTree();
	void load(const MeshBuilder& builder);
	bool ray_intersect_with_mesh(const Ray& ray, int k = 1);
};

#endif
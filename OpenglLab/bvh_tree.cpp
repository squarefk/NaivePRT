#include "bvh_tree.h"

BvhTree::Triangle::Triangle(glm::vec3 _p, glm::vec3 _q, glm::vec3 _r) {
	p = _p;
	q = _q;
	r = _r;
}

BvhTree::Ray::Ray(glm::vec3 _pos, glm::vec3 _dir) {
	pos = _pos;
	dir = _dir;
	inv = glm::vec3(1.f / dir.x, 1.f / dir.y, 1.f / dir.z);
}

BvhTree::BvhTree() {

}

BvhTree::~BvhTree() {
	delete[] t;
	delete[] queue;
}

void BvhTree::build(int k, int l, int r) {
	t[k].l = l;
	t[k].r = r;
	t[k].m = (l + r) >> 1;
	t[k].low = t[k].high = triangles[l].p;
	for (int i = l; i <= r; ++i) {
		const Triangle& tri = triangles[i];
		t[k].low.x = std::min(std::min(t[k].low.x, tri.p.x), std::min(tri.q.x, tri.r.x));
		t[k].low.y = std::min(std::min(t[k].low.y, tri.p.y), std::min(tri.q.y, tri.r.y));
		t[k].low.z = std::min(std::min(t[k].low.z, tri.p.z), std::min(tri.q.z, tri.r.z));
		t[k].high.x = std::max(std::max(t[k].high.x, tri.p.x), std::max(tri.q.x, tri.r.x));
		t[k].high.y = std::max(std::max(t[k].high.y, tri.p.y), std::max(tri.q.y, tri.r.y));
		t[k].high.z = std::max(std::max(t[k].high.z, tri.p.z), std::max(tri.q.z, tri.r.z));
	}
	float lenx = t[k].high.x - t[k].low.x;
	float leny = t[k].high.y - t[k].low.y;
	float lenz = t[k].high.z - t[k].low.z;
	auto cmpx = [](const Triangle& a, const Triangle& b) {
		return std::max(a.p.x, std::max(a.q.x, a.r.x)) < std::max(b.p.x, std::max(b.q.x, b.r.x));
	};
	auto cmpy = [](const Triangle& a, const Triangle& b) {
		return std::max(a.p.y, std::max(a.q.y, a.r.y)) < std::max(b.p.y, std::max(b.q.y, b.r.y));
	};
	auto cmpz = [](const Triangle& a, const Triangle& b) {
		return std::max(a.p.z, std::max(a.q.z, a.r.z)) < std::max(b.p.z, std::max(b.q.z, b.r.z));
	};
	if (lenx > leny && lenx > lenz) {
		std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpx);
	}
	else {
		if (leny > lenz)
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpy);
		else
			std::sort(triangles.begin() + l, triangles.begin() + r + 1, cmpz);
	}
	/*
	if (k < 100) {
		fprintf(stderr, "%d %d %d (%.2f %.2f %.2f) (%.2f %.2f %.2f)\n",
			k,t[k].l,t[k].r,
			t[k].low.x, t[k].low.y, t[k].low.z,
			t[k].high.x, t[k].high.y, t[k].high.z);
	}
	*/
	if (l == r) return;
	build(k + k, l, t[k].m);
	build(k + k + 1, t[k].m + 1, r);
}

void BvhTree::load(const MeshBuilder& builder) {
	fprintf(stderr, "Start building BVH-tree.\n");
	for (unsigned int i = 0; i < builder.indicesData.size(); i += 3) {
		int x = builder.indicesData[i] * 3;
		int y = builder.indicesData[i + 1] * 3;
		int z = builder.indicesData[i + 2] * 3;
		triangles.push_back(Triangle(
			glm::vec3(builder.positionData[x], builder.positionData[x + 1], builder.positionData[x + 2]),
			glm::vec3(builder.positionData[y], builder.positionData[y + 1], builder.positionData[y + 2]),
			glm::vec3(builder.positionData[z], builder.positionData[z + 1], builder.positionData[z + 2])
		));
	}
	t = new Node[triangles.size() * 4];
	queue = new int[triangles.size() * 4];
	build(1, 0, triangles.size() - 1);
	fprintf(stderr, "Finish building BVH-tree successfully.\n");
}

bool BvhTree::ray_intersect_with_triangle(const Ray& ray, const Triangle& tri) {
	float t, u, v;
	glm::vec3 E1 = tri.q - tri.p;
	glm::vec3 E2 = tri.r - tri.p;
	glm::vec3 P = glm::cross(ray.dir, E2);
	float det = glm::dot(E1, P);
	glm::vec3 T;
	if (det > 0) {
		T = ray.pos - tri.p;
	}
	else {
		T = tri.p - ray.pos;
		det = -det;
	}
	static float eps = 1e-5f;
	if (det < eps) return false;
	u = glm::dot(T, P);
	if (u < -eps || u > det + eps) return false;
	glm::vec3 Q = glm::cross(T, E1);
	v = glm::dot(ray.dir, Q);
	if (v < -eps || u + v > det + eps) return false;
	t = glm::dot(E2, Q);
	if (t < det * 1e-5f) return false;
	dist = t / det;
	return true;
}

bool BvhTree::ray_intersect_with_box(const Ray& ray, const glm::vec3& low, const glm::vec3& high) {
	float tmin = -1e30f, tmax = 1e30f;
	float _tmin, _tmax;
	bool sign;
#define TEST(t)\
	if (fabs(ray.dir.t)>1e-4f)\
	{\
		sign=(ray.inv.t<0);\
		_tmin=((sign?high.t:low.t)-ray.pos.t)*ray.inv.t;\
		_tmax=((sign?low.t:high.t)-ray.pos.t)*ray.inv.t;\
		if (_tmin > tmin) tmin=_tmin;\
		if (_tmax < tmax) tmax=_tmax;\
		if (tmin > tmax + 1e-4f) return false;\
	}
	TEST(x); TEST(y); TEST(z);
#undef TEST
	return true;
}

bool BvhTree::ray_intersect_with_mesh(const Ray& ray, int k) {
	queue[1] = 1;
	int rr;
	for (int l = 1, r = 1; l <= r; ++l) {
		int k = queue[l];
		if (t[k].l == t[k].r) {
			if (ray_intersect_with_triangle(ray, triangles[t[k].l])) {				
				return true;
			} 
			else
				continue;
		}
		if (!ray_intersect_with_box(ray, t[k].low, t[k].high)) {
			continue;
		}
		queue[++r] = k + k;
		queue[++r] = k + k + 1;
		rr = r;
	}
//	fprintf(stderr, "The cost is %d\n", rr);
	return false;
}
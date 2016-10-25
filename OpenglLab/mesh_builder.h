#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#define  GLEW_STATIC
#include <GL/glew.h>
#include <glfw/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>

class MeshBuilder{
	std::vector<float> positionData;
	std::vector<float> colorData;
	std::vector<float> normalData;
	std::vector<unsigned int> indicesData;

	GLuint vertexArrayID;
	GLuint vertexBufferPositionID;
	GLuint vertexBufferColorID;
	GLuint vertexBufferNormalID;
	GLuint vertexBufferIndicesID;

	void processNode(const aiNode* node, const aiScene* sceneObjPtr);
	void processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr);
	void completeNormalAndColor();
	void generateVAO();
public:
	bool import(const std::string& pFile);
};

#endif
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
public:
	// must use indicesData[i] * 3 to calc index in positionData
	std::vector<float> positionData;
	std::vector<float> colorData;
	std::vector<float> normalData;
	std::vector<unsigned int> indicesData;

private:
	GLuint vertexArrayID;
	GLuint vertexBufferPositionID;
	GLuint vertexBufferColorID;
	GLuint vertexBufferNormalID;
	GLuint vertexBufferIndicesID;
	void processNode(const aiNode* node, const aiScene* sceneObjPtr);
	void processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr);
	void completeNormalAndColor();

public:
	float import(const std::string& pFile = std::string(""), int extraObj = 0);
	void generateVAO();
	void draw();
	void draw0();
};

#endif
#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshBuilder{
	void processNode(const aiNode* node, const aiScene* sceneObjPtr);
	void processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr);
public:
	bool import(const std::string& pFile);
};

#endif
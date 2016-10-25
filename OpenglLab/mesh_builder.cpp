#include "mesh_builder.h"

bool MeshBuilder::import(const std::string& pFile) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		pFile,
		aiProcess_CalcTangentSpace |		//�����־���Զ��������ߺ͸�����
		aiProcess_Triangulate |				//�����־���Զ����ı�����ת��Ϊ������
		aiProcess_JoinIdenticalVertices |	//�����־���Զ��ϲ���ͬ�Ķ���
		aiProcess_SortByPType				//�����־������ͬͼԪ���õ���ͬ��ģ����ȥ��ͼƬ���Ϳ����ǵ㡢ֱ�ߡ������ε�
	);
	if (!scene ||
		scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		fprintf(stderr, (std::string("Error occurs when import obj from ") + pFile).c_str());
		return false;
	}
	processNode(scene->mRootNode, scene);
	return true;
}

void MeshBuilder::processNode(const aiNode* node, const aiScene* sceneObjPtr) {
	for (size_t i = 0; i < node->mNumMeshes; ++i) {
		const aiMesh* meshPtr = sceneObjPtr->mMeshes[node->mMeshes[i]];
		this->processMesh(meshPtr, sceneObjPtr);
	}
	for (size_t i = 0; i < node->mNumChildren; ++i)	{
		this->processNode(node->mChildren[i], sceneObjPtr);
	}
}

void MeshBuilder::processMesh(const aiMesh* meshPtr, const aiScene* sceneObjPtr)
{
	for (size_t i = 0; i < meshPtr->mNumVertices; ++i) {
	}
	for (size_t i = 0; i < meshPtr->mNumFaces; ++i) {
		for (int j = 0; j < 3; ++j) {
			aiVector3D v = meshPtr->mVertices[meshPtr->mFaces[i].mIndices[j]];
			fprintf(stderr, "(%.2f %.2f %.2f) ", v.x, v.y, v.z);
		}
		fprintf(stderr,"\n");
	}
/*
	if (meshPtr->mMaterialIndex >= 0)
	{
		processMaterial(materialPtr, sceneObjPtr, aiTextureType_DIFFUSE, diffuseTexture);
		processMaterial(materialPtr, sceneObjPtr, aiTextureType_SPECULAR, specularTexture);
	}
	http://blog.csdn.net/wangdingqiaoit/article/details/52014321
*/
}
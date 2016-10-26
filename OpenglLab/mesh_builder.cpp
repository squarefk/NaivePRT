#include "mesh_builder.h"

#include <glm/glm.hpp>

bool MeshBuilder::import(const std::string& pFile) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(
		pFile,
		aiProcess_CalcTangentSpace |		//后处理标志，自动计算切线和副法线
		aiProcess_Triangulate |				//后处理标志，自动将四边形面转换为三角面
		aiProcess_JoinIdenticalVertices |	//后处理标志，自动合并相同的顶点
		aiProcess_SortByPType				//后处理标志，将不同图元放置到不同的模型中去，图片类型可能是点、直线、三角形等
	);
	if (!scene ||
		scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE ||
		!scene->mRootNode) {
		fprintf(stderr, (std::string("Error occurs when import obj from ") + pFile).c_str());
		return false;
	}
	processNode(scene->mRootNode, scene);

	std::vector<float> groundPositionData = std::vector<float>{
		-3.f,-1.f,-3.f,
		3.f,-1.f,-3.f,
		-3.f,-1.f,3.f,
		3.f,-1.f,3.f,
	};
	unsigned int index = positionData.size() / 3;
	std::vector<unsigned int> indicesPositionData = std::vector<unsigned int> {
		index, index + 2, index + 1,
		index + 1, index + 2, index + 3
	};
	for (auto f : groundPositionData) {
		positionData.push_back(f);
	}
	for (auto f : indicesPositionData) {
		indicesData.push_back(f);
	}

	completeNormalAndColor();
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
		// push_back position of vertex
		positionData.push_back(meshPtr->mVertices[i].x);
		positionData.push_back(meshPtr->mVertices[i].y);
		positionData.push_back(meshPtr->mVertices[i].z);
		// push_back color of vertex
		if (meshPtr->mColors[0] != NULL) {
			colorData.push_back(meshPtr->mColors[0][i].r);
			colorData.push_back(meshPtr->mColors[0][i].g);
			colorData.push_back(meshPtr->mColors[0][i].b);
		}
		// push_back normal of vertex
		if (meshPtr->mNormals != NULL) {
			normalData.push_back(meshPtr->mNormals[i].x);
			normalData.push_back(meshPtr->mNormals[i].y);
			normalData.push_back(meshPtr->mNormals[i].z);
		}
	}
	for (size_t i = 0; i < meshPtr->mNumFaces; ++i) {
		indicesData.push_back(meshPtr->mFaces[i].mIndices[0]);
		indicesData.push_back(meshPtr->mFaces[i].mIndices[1]);
		indicesData.push_back(meshPtr->mFaces[i].mIndices[2]);
		/*
		fprintf(stderr, "%d %d %d\n",
			meshPtr->mFaces[i].mIndices[0],
			meshPtr->mFaces[i].mIndices[1],
			meshPtr->mFaces[i].mIndices[2]);
		*/
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

void MeshBuilder::completeNormalAndColor() {
	int tempColorDataSize = colorData.size();
	for (int i = tempColorDataSize; i < positionData.size(); i += 3) {
		colorData.push_back(1.f);
		colorData.push_back(1.f);
		colorData.push_back(1.f);
	}
	int tempNormalDataSize = normalData.size();
	for (int i = tempNormalDataSize; i < positionData.size(); i += 3) {
		normalData.push_back(0.f);
		normalData.push_back(0.f);
		normalData.push_back(0.f);
	}
	for (int i = 0; i < indicesData.size(); i += 3)
		if (indicesData[i] * 3 >= tempNormalDataSize) {
			glm::vec3 v[3];
			for (int delta = 0; delta < 3; ++delta) {
				v[delta].x = positionData[indicesData[i + delta] * 3];
				v[delta].y = positionData[indicesData[i + delta] * 3 + 1];
				v[delta].z = positionData[indicesData[i + delta] * 3 + 2];
			}
			glm::vec3 n = glm::normalize(glm::cross(v[1] - v[0], v[2] - v[0]));
			for (int delta = 0; delta < 3; ++delta) {
				normalData[indicesData[i + delta] * 3] += n.x;
				normalData[indicesData[i + delta] * 3 + 1] += n.y;
				normalData[indicesData[i + delta] * 3 + 2] += n.z;
			}
		}
	for (int i = tempNormalDataSize; i < positionData.size(); i += 3) {
		glm::vec3 n = glm::vec3(
			normalData[i],
			normalData[i + 1],
			normalData[i + 2]
		);
		n = glm::normalize(n);
		normalData[i] = n.x;
		normalData[i + 1] = n.y;
		normalData[i + 2] = n.z;
	}
	
	/*
	for (int i = 0; i < normalData.size(); i += 3)
		fprintf(stderr,"The normal of (%.2f %.2f %.2f) is (%.2f %.2f %.2f)\n",
			positionData[i],
			positionData[i + 1],
			positionData[i + 2],
			normalData[i],
			normalData[i + 1],
			normalData[i + 2]
		);
	*/
}

void MeshBuilder::generateVAO() {
	// generate a VAO
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	// generate a VBO for position
	glGenBuffers(1, &vertexBufferPositionID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferPositionID);
	glBufferData(GL_ARRAY_BUFFER, positionData.size() * sizeof(float), &positionData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// generate a VBO for color
	glGenBuffers(1, &vertexBufferColorID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferColorID);
	glBufferData(GL_ARRAY_BUFFER, colorData.size() * sizeof(float), &colorData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// generate a VBO for normal
	glGenBuffers(1, &vertexBufferNormalID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferNormalID);
	glBufferData(GL_ARRAY_BUFFER, normalData.size() * sizeof(float), &normalData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// generate a VBO for indices
	glGenBuffers(1, &vertexBufferIndicesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferIndicesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesData.size() * sizeof(unsigned int), &indicesData[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
}

void MeshBuilder::draw() {
	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferIndicesID);
	glDrawElements(GL_TRIANGLES, indicesData.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glBindVertexArray(0);
}
void MeshBuilder::draw0() {
	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferIndicesID);
	glDrawElements(GL_TRIANGLES, indicesData.size(), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}
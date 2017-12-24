#pragma once

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <common/mesh.h>

#include <string>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Model
{
public:
	vector<Mesh> meshes;

	explicit Model(string const& path)
	{
		load_model(path);
	}

	void draw()
	{
		for (unsigned int i = 0; i < meshes.size(); i++)
			meshes[i].draw();
	}

private:
	void load_model(string const& path)
	{
		Assimp::Importer importer;
		const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
			return;
		}

		process_node(scene->mRootNode, scene);
	}

	void process_node(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			const auto mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(process_mesh(mesh));
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			process_node(node->mChildren[i], scene);
		}
	}

	static Mesh process_mesh(aiMesh* mesh)
	{
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
			vertices.push_back(vertex);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const auto face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Mesh(vertices, indices);
	}
};


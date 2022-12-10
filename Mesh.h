#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "VAO.h"
#include "EBO.h"
//#include "shaderClass.h"
#include "Camera.h"
#include "Texture.h"

class Mesh
{
public:
	std::vector <Vertex> vertices;
	std::vector <GLuint> indices;
	std::vector <Texture> textures;

	VAO VAO;

	/// <summary>
	/// Number of instances of THIS mesh
	/// </summary>
	unsigned int instancing;

	Mesh
	(
		std::vector <Vertex>& vertices,
		std::vector <GLuint>& indices,
		std::vector <Texture>& textures,
		unsigned int instancing = 1,
		std::vector<glm::mat4> instanceMatrices = {}
	);

	bool syncTime = false;

	void Draw(
		Shader& shader,
		Camera& camera,
		glm::mat4 matrix = glm::mat4(1),
		glm::vec3 translation = glm::vec3(0, 0, 0),
		glm::quat rotation = glm::quat(1, 0, 0, 0),
		glm::vec3 scale = glm::vec3(1, 1, 1)
		);
};


#endif
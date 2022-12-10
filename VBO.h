#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>
#include  <glm/glm.hpp>

#include <vector>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texUV;
};

class VBO
{
public:
	GLuint ID;
	VBO(std::vector<Vertex>& vertices);
	/// <summary>
	/// Transformations that we want to pass to EACH INSTANCED MESH
	/// </summary>
	/// <param name="mat4s">Transformations</param>
	VBO(std::vector<glm::mat4>& mat4s);

	void Bind();
	void Unbind();
	void Delete();
};

#endif

#include "EBO.h"

EBO::EBO(std::vector<GLuint>& indices)
{
	glGenBuffers(1, &ID);
	// Select our EBO as an ELEMENT_ARRAY
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	// Store our indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}

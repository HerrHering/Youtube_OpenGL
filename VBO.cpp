#include "VBO.h"

VBO::VBO(std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &ID);

	// We bind VBO AFTER the VAO!!!
	glBindBuffer(GL_ARRAY_BUFFER, ID);

	// (ARRAY TYPE, size of the buffer, data to store, USAGE OF DATA)
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}

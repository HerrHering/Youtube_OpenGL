#include "VAO.h"

/// <summary>
/// A VAO is an object that wraps vertices and vertex indices together to form a mesh later on
/// </summary>
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

/// <summary>
/// Links a VBO to a VAO
/// For further help: BufferConstrucion.PNG
/// </summary>
/// <param name="VBO">The VBO we want to link / want to read from</param>
/// <param name="layout">Target layout in shader</param>
/// <param name="numComopnents">Number of vertices</param>
/// <param name="type">Type of data inside buffer (only 1type supported)</param>
/// <param name="stride">Distance in bytes between vertices</param>
/// <param name="offset">Offset in bytes inside vertex (a vertex can hold multiple values)</param>
void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComopnents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();

	glVertexAttribPointer(layout, numComopnents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);

	VBO.Unbind();
}

void VAO::Bind()
{
	glBindVertexArray(ID);
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}

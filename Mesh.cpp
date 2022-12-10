#include "Mesh.h"


Mesh::Mesh
(
	std::vector<Vertex>& vertices,
	std::vector<GLuint>& indices,
	std::vector<Texture>& textures,
	unsigned int instancing,
	std::vector<glm::mat4> instanceMatrices
)
{
	Mesh::vertices = vertices;
	Mesh::indices = indices;
	Mesh::textures = textures;
	Mesh::instancing = instancing;

	VAO.Bind();

	// Create a VBO to store the transformation of each mesh instance
	VBO instanceVBO(instanceMatrices);
	// Create VertexBuffer and saves it into our VAO
	VBO vertexVBO(vertices);
	// Create IndexBuffer and saves it into our VAO
	EBO EBO(indices);

	// Link the VertexBuffer to our VAO
	// POSITION * 3f
	VAO.LinkAttrib(vertexVBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	// NORMALS * 3f
	VAO.LinkAttrib(vertexVBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	// COLOR * 3f
	VAO.LinkAttrib(vertexVBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	// UV * 3f
	VAO.LinkAttrib(vertexVBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	// Do the instancing stuff iff we want to draw more than one mesh
	if (instancing != 1)
	{
		// We want to pass in a 4x4 matrix to the shader
		// Can't link to a mat4 so you need to link four vec4s
		VAO.LinkAttrib(instanceVBO, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		VAO.LinkAttrib(instanceVBO, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		VAO.LinkAttrib(instanceVBO, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		VAO.LinkAttrib(instanceVBO, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

		// (layout, divisor)
		// we want to compress the four vec4s into one mat4
		// we want to apply the SAME mat4 for all vertices of the SAME mesh
		// Solution:
		//		We can tell OpenGL, how often should it advance on specific attribute slots
		//		(slot, 0): we want to advance to the next value when we move from vertex to vertex
		//		(slot, 1): we want to advance this slot's value only when we move from instance to instance
		//		(slot, divisor), divisor != 0: we want to advance this slot's value after every {divisor} instances
		// In our example, we want to advance to the next transformation matrix only when we move to drawing the next instance
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
	}

	// Unbind unused resources
	VAO.Unbind();
	vertexVBO.Unbind();
	instanceVBO.Unbind();
	EBO.Unbind();
}

void Mesh::Draw
(
	Shader& shader,
	Camera& camera,
	glm::mat4 matrix,
	glm::vec3 translation,
	glm::quat rotation,
	glm::vec3 scale
)
{
	shader.Activate();
	VAO.Bind();

	// Texture nums
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		std::string num;
		std::string type = textures[i].type;

		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}

		// diffuse0, diffuse1, diffuse2...
		// specular0, specular1, specular2...
		textures[i].texUnit(shader, (type + num).c_str(), i);
		textures[i].Bind();
	}

	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
	camera.Matrix(shader, "camMatrix");

	// Custom data, not part of the mesh
	if (this->syncTime)
	{
		glUniform1f(glGetUniformLocation(shader.ID, "_Time"), glfwGetTime());
	}

	// Check if we are drawing normally or instancing
	if (instancing == 1)
	{

		// Initialize matrices
		glm::mat4 trans = glm::mat4(1);
		glm::mat4 rot = glm::mat4(1);
		glm::mat4 sca = glm::mat4(1);

		// Calculate the transformation matrices of the mesh, so we can place it in space wherever we want
		trans = glm::translate(trans, translation);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);

		// Set data for the mesh in VERTEX SHADER
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
	
		// Draw mesh
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{

		// Actually draw the mesh after setting up everything
		// The instancing version requires the number of times we want to draw the EXACT SAME mesh
		// 
		// unique "gl_InstanceID" for every instanced mesh -> retrieve correct transformation / generate transformation (IN THE SHADER -> SLOW)
		// Problem: a uniform array can only hold ~15000 transformations
		// Solution: Send the transformations to the GPU through a VBO & VAO!
		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, instancing);
	}
}

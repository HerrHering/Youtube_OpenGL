// https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
#include <stb/stb_image.h>
// https://glm.g-truc.net/0.9.9/index.html
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Camera.h"

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

#pragma region RANDOM CONSTANT VALUES
GLfloat vertices[] =
{
//		COORDINATES		/			COLORS			/			UV			//
	-0.5f,	0.0f,	0.5f,		0.83f,	0.70f,	0.44f,		0.0f,	0.0f,	// Top left
	-0.5f,	0.0f,	-0.5f,		0.83f,	0.70f,	0.44f,		5.0f,	0.0f,	// Bottom left
	0.5f,	0.0f,	-0.5f,		0.83f,	0.70f,	0.44f,		0.0f,	0.0f,	// Bottom right
	0.5f,	0.0f,	0.5f,		0.83f,	0.70f,	0.44f,		5.0f,	0.0f,	// Top right
	0.0f,	0.8f,	0.0f,		0.92f,	0.86f,	0.76f,		2.5f,	5.0f,	// CSÚCS
};

GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4,
};
#pragma endregion


int main()
{
#pragma region INITIALIZATION AND BASIC WINDOW SETTINGS
	// Initialize graphics librari
	glfwInit();

	// Tell the computer which version we are using (OpenGl 3.3)
	// Before the dot
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// After the dot
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// What function package are we using: MODERN<- / COMPATIBILITY
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// CREATE WINDOW OBJECT

	// Create the window (windowWidth, windowHeight, windowTitle, defaultMonitor, dontShareResources)
	GLFWwindow* window = glfwCreateWindow(800, 800, "YoutubeOpenGL", NULL, NULL); // By NULL we use the default settings
	// Check wether the window has been succesfully created
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Tells OpenGL that we want to use this window for the context
	glfwMakeContextCurrent(window);

	// Load GLAD so it configures OpenGL
	// Loads GL functions, headers and other configurations
	gladLoadGL();

	// Set viewport coordinates from bottom-left to top-right
	glViewport(0, 0, windowWidth, windowHeight);
#pragma endregion

#pragma region SET UP SHADERS AND BUFFERS
	
	// Create a shaderprogram, that creates a pipeline from a VERTEX and a FRAGMENT shader
	Shader shaderProgram("default.vert", "default.frag");

	// Create VAO
	VAO VAO1;
	VAO1.Bind();

	// Create VertexBuffer and saves it into our VAO
	VBO VBO1(vertices, sizeof(vertices));
	// Create IndexBuffer and saves it into our VAO
	EBO EBO1(indices, sizeof(indices));

	// Link the VertexBuffer to our VAO
	// POSITION * 3f
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	// COLOR * 3f
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	// UV * 2f
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	// Unbind unused resources
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// TEXTURE
	// (image name, texture type, unit in the "texture boundle", image pixelformat, image pixel dataformat)
	Texture brickPyramid("pop_cat.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);

	// Set Texture
	// (program, uniform name, texture index in boundle)
	brickPyramid.texUnit(shaderProgram, "tex0", 0);
#pragma endregion

#pragma region RENDERING AND WINDOWHANDLING
	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	// We are in 3D, so we have to know what is furher away (we can't see behind objects)
	glEnable(GL_DEPTH_TEST);

	// Processes all incoming events related to our window
	while (!glfwWindowShouldClose(window))
	{
		// Tell GL to clear back buffer with specific color
		glClearColor(.07f, .13f, .17f, 1.0f);
		// Clears COLOR_BUFFER with the previously set clearColor
		// Clear depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Now is the time that the computer runs our shaderprogram :)
		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.1f, 100.0f, shaderProgram, "camMatrix");

		// Bind texture
		brickPyramid.Bind();

		// We tell the computer, which VAO to use
		VAO1.Bind();

		// Draw triangles
		// (PRIMITIVE_TYPE, INDEX_COUNT, INDEX_TYPE, STARTINDEX_OF_INDICES)
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		// Swaps the front and back buffers
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	brickPyramid.Delete();
	shaderProgram.Delete();

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
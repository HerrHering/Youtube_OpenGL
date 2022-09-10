// https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org
// 29 : 00

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

const char* vertexShaderSource =
{
	"#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\n\0"
};
const char* fragmentShaderSource =
{
	"#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0);\n"
	"}\n\0"
};

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
	glViewport(0, 0, 800, 800);
#pragma endregion

#pragma region SET UP SHADERS
	// We will be able to find our vertex shader by the ID stored in it (by reference)
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// (ID of the shader, how many strings are we using for THIS SPECIFIC shader, where to read from, DEFAULT)
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	// Compiles the text it read as a usable shadercode (ID of the shader)
	glCompileShader(vertexShader);

	// We will be able to find our fragment shader by the ID stored in it (by reference)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// (ID of the shader, how many strings are we using for THIS SPECIFIC shader, where to read from, DEFAULT)
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	// Compiles the text it read as a usable shadercode (ID of the shader)
	glCompileShader(fragmentShader);

	// Creates a shader program
	// The computer will run this and we can store our shaders in it
	GLuint shaderProgram = glCreateProgram();
	// We attach our shaders to the shaderprogram so it will run on the GPU
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Link all the shaders together into the shader program
	// Create a rendering pipeline for the shader program: FIRST: VERTEX, SECOND: FRAGMENT, etc...
	glLinkProgram(shaderProgram);

	// The compiled shaders are already stored in the shaderprogram so we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
#pragma endregion

#pragma region CREATE BUFFER DATA
	// Create an array of vertex positions (X, Y, Z)
	GLfloat vertices[] =
	{
		-.5f, -.5f * float(sqrt(3)) / 3, 0.0f,
		.5f, -.5f * float(sqrt(3)) / 3, 0.0f,
		0.0f, .5f * float(sqrt(3)) * 2 / 3, 0.0f,
	};

	// Vertex Buffer Object
	GLuint VBO;
	// Generates an empty VBO (NUM OBJECTS, ID)
	glGenBuffers(1, &VBO);
	// Vertex Array Object
	// Stores references to VBO-s and tells OpenGL how to interpret them
	// OpenGL will search for data here
	GLuint VAO;
	// Generates empty VAO (NUM OBJECTS, ID)
	glGenVertexArrays(1, &VAO);

	// We need to bind VAO first, so it can see what is stored into the VBO!!!
	// Select (Bind) our VAO so we can operate on it
	glBindVertexArray(VAO);
	// Bind == Select
	// We bind VBO AFTER the VAO!!!
	// We select our vertex buffer obj and operate on it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set data to the vertex buffer obj
	// (ARRAY TYPE, size of the buffer, data to store, USAGE OF DATA)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Define an attribute of vertex shader (for example take a look at the shader files)
	// This attr. will be targeted by our VBO (BECAUSE IT IS THE ONE BOUND RIGHT NOW)
	// (Position of vertex attribute, num of data per vertex, DATA TYPE, ???, size of each element, offset to vertices (they arent always in the beginning of the array) )
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	// Enables the computer to send data to this attribute (location = 0)
	glEnableVertexAttribArray(0);

	// FIRST we need to unbind the 
	// Tell OpenGL to bind "0" as THE buffer (0 == nothing is selected)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Tell OpenGL to bind "0" as THE vertex array (0 == nothing is selected)
	glBindVertexArray(0);
#pragma endregion


#pragma region BUFFER AND RENDER STUFF

	// Processes information about the window
	// This will terminate if, for example, we close the window
	// Untill then it will run and display what we want
	// Here will we be able to process user input etc...
	// This will give the main functionality of the program
	while (!glfwWindowShouldClose(window))
	{
		// Tell GL to clear back buffer with specific color
		glClearColor(.07f, .13f, .17f, 1.0f);
		// Clears COLOR_BUFFER with the previously set clearColor
		glClear(GL_COLOR_BUFFER_BIT);

		// Now is the time that the computer runs our shaderprogram :)
		glUseProgram(shaderProgram);
		// We tell the computer, which VAO to use
		glBindVertexArray(VAO);
		// Tell ObenGL to draw
		// (PRIMITIV TYPE, where does the indexing start of the vertices, how many vertices to use when drawing ONE primitiv)
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Swaps the front and back buffers
		// We cleared the back buffer previously by a clearColor, and now we will display it
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
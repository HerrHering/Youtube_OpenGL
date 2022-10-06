// https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org

#include "Mesh.h"

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

#pragma region RANDOM CONSTANT VALUES
// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           TexCoord         /       NORMALS         //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
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

	// MESH
	Texture textures[]
	{
		Texture("planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),

		Texture("planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE),
	};

	// Create a shaderprogram, that creates a pipeline from a VERTEX and a FRAGMENT shader
	Shader meshShader("default.vert", "default.frag");

	std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector<Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));

	// LIGHT
	Mesh floor(verts, ind, tex);

	Shader lightShader("light.vert", "light.frag");

	std::vector<Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector<GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	// The texture parameter is jut a placeholder
	Mesh light(lightVerts, lightInd, tex);

#pragma region SET UP SCENE

	// SCENE DATA
	glm::vec4 lightColor = glm::vec4(1, 1, 1,	 1);

	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	glm::vec3 pyramidPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 pyramidModel = glm::mat4(1.0f);
	pyramidModel = glm::translate(pyramidModel, pyramidPos);

	// SET SHADERS
	lightShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
	glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);

	meshShader.Activate();
	glUniformMatrix4fv(glGetUniformLocation(meshShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(pyramidModel));
	glUniform4f(glGetUniformLocation(meshShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(meshShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

#pragma endregion

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

		// Camera inputs: moving, rotating...
		camera.Inputs(window);
		// updates the cameraMatrix
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		floor.Draw(meshShader, camera);
		light.Draw(lightShader, camera);

		// Swaps the front and back buffers
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS
	meshShader.Delete();
	lightShader.Delete();

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
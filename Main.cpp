 // https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org

// Episode: https://youtu.be/ngF9LWWxhd0

#include "Model.h"

const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

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



#pragma region SET UP SCENE

	Shader meshShader("default.vert", "default.frag");

	Shader outlineShader("outline.vert", "outline.frag");

	glm::vec4 lightColor = glm::vec4(1, 1, 1,	 1);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	meshShader.Activate();
	glUniform4f(glGetUniformLocation(meshShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(meshShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

#pragma endregion

#pragma region RENDERING AND WINDOWHANDLING
	// We are in 3D, so we have to know what is furher away (we can't see behind objects)
	glEnable(GL_DEPTH_TEST);
	// Default depthbuffer usage: if something has a smaller depth value than the current, it will replace it
	glDepthFunc(GL_LESS);

	// Enable THE stencil buffer: 8bit
	glEnable(GL_STENCIL_TEST);
	// Results if: (stencil fails), (depth fails), (depth passes)
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	Model modelGround("models/ground/scene.gltf");
	Model modelTrees("models/trees/scene.gltf");

	// Processes all incoming events related to our window
	while (!glfwWindowShouldClose(window))
	{
		// Tell GL to clear back buffer with specific color
		glClearColor(0.85f, 0.85f, 0.90f, 1.0f);
		// Clears COLOR_BUFFER with the previously set clearColor
		// Clear depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		// Camera inputs: moving, rotating...
		camera.Inputs(window);
		// updates the cameraMatrix
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// Draw models

		// glStencilFunc:
		// Condition to pass the test (also, GL_REPLACE will set the stencil value to REF if it passes)
		// (condition to pass), (ref), (mask = which bits are we using)
		// Which bits are we using: all=0xFF, nothing=0x00						(stencil & mask)
		// 
		// glStencilMask:
		// Which bits are we using? 0xFF:all, 0x00:nothing
		// We can only change the bits in usage

		modelGround.Draw(meshShader, camera);

		// The stencil test will always pass, and ref = 1 -> the stencilbuff will be 1 where the obj is on the screen
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		// We use 8bits
		glStencilMask(0xFF);
		modelTrees.Draw(meshShader, camera);

		// Only passes if stencil != ref, and ref = 1 -> where our original obj was
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		// No more writing to the stencilbuff
		glStencilMask(0x00);

		outlineShader.Activate();
		// The outlining value is a scaling "outwards the surface of the obj" (we inflate the original obj)
		glUniform1f(glGetUniformLocation(outlineShader.ID, "outlining"), 0.08f);
		// Re-draw the same object, except using the outlining shader
		// Because of the stencil condition, only the part will be drawn, where the original and upscaled obj dont ovelap
		//modelGround.Draw(outlineShader, camera);
		modelTrees.Draw(outlineShader, camera);

		// RESET STENCIL BUFFER
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);

		// Swaps the front and back buffers
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS
	meshShader.Delete();

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
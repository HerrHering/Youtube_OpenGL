 // https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org

// Episode: https://youtu.be/crOfyWiWxmc

#include "Model.h"

// Proceduraly draw windows
const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

float rectangleVertices[] =
{
	// Coords    // texCoords
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f,

	 1.0f,  1.0f,  1.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	-1.0f,  1.0f,  0.0f, 1.0f
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
	glViewport(0, 0, windowWidth, windowHeight);
#pragma endregion



#pragma region SET UP SCENE

	Shader meshShader("default.vert", "default.frag");

	Shader frameBufferShader("framebuffer.vert", "framebuffer.frag");

	glm::vec4 lightColor = glm::vec4(1, 1, 1,	 1);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	meshShader.Activate();
	glUniform4f(glGetUniformLocation(meshShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(meshShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	frameBufferShader.Activate();
	glUniform1i(glGetUniformLocation(frameBufferShader.ID, "screenTexture"), 0);


#pragma endregion

#pragma region RENDERING AND WINDOWHANDLING
	// We are in 3D, so we have to know what is furher away (we can't see behind objects)
	glEnable(GL_DEPTH_TEST);
	// Default depthbuffer usage: if something has a smaller depth value than the current, it will replace it
	glDepthFunc(GL_LESS);

	// ONLY draws specified side of triangle
	glEnable(GL_CULL_FACE);
	// Draw fornt face
	glCullFace(GL_FRONT);
	// Definition of front face: counter clockwise indexing
	glFrontFace(GL_CCW);

	// Enable THE stencil buffer: 8bit
	glEnable(GL_STENCIL_TEST);
	// Results if: (stencil fails), (depth fails), (depth passes)
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Blending: we put layers of colors onto eachother and interpolate between them based on alpha value
	// (alpha value source for the newly computed color), (alpha value for the color in the colorbuffer)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load in models
	Model model("models/crow/scene.gltf");


	unsigned int rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	// FPS COUNTER
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// How many frames in a certain amount of time
	unsigned int frameCounter = 0;
	const double frameMeasurementPerSecond = 60;

	// Create framebuffer object
	// This is an object that contains the colorBuffer, depthBuffer and stencilBuffer
	unsigned int FBO;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	// Create test texture
	// We attach a texture to the FBO so it can draw into it, and we can process it with our custom postp. shader
	unsigned int frameBufferTexture;
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	// Render buffer object
	// This is a buffer attached to the FBO, that cannot be modified with an extra custom shader
	// In our case this is good for the depthBuffer because we dont want to use it in our sahder 
	unsigned int RBO;
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	// Attach renderbuffer to framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	// Error checking
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

	// Processes all incoming events related to our window
	while (!glfwWindowShouldClose(window))
	{
#pragma region FPS counter

		// The FPS counter runs on a fixed timeinterval
		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		frameCounter++;
		if (timeDiff >= 1.0 / frameMeasurementPerSecond)
		{
			std::string FPS = std::to_string(frameCounter / timeDiff);
			std::string frameDuration = std::to_string(timeDiff / frameCounter * 1000);
			std::string newTitle = "YoutubeOpenGL - " + FPS + "FPS / " + frameDuration + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());
			frameCounter = 0;
			prevTime = crntTime;
		}

#pragma endregion

		// Tells OpenGL that we want it to save the buffers' values into this framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, FBO);

		// Tell GL to clear back buffer with specific color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clears COLOR_BUFFER with the previously set clearColor
		// Clear depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		// Camera inputs: moving, rotating...
		camera.Inputs(window);
		// updates the cameraMatrix
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// DRAW
		model.Draw(meshShader, camera);

		// After drawing the scene, we wont include anything into our frameBuffer after this
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Activate out custom shader
		frameBufferShader.Activate();
		// We want to draw a rectangle that cowers the whole screen
		glBindVertexArray(rectVAO);
		// We dont want to change the depth values, only read them
		glDisable(GL_DEPTH_TEST);
		glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Swaps the front and back buffers
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS
	meshShader.Delete();
	frameBufferShader.Delete();

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
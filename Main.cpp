 // https://www.youtube.com/watch?v=45MIykWJ-C4&ab_channel=freeCodeCamp.org

// Episode: https://youtu.be/crOfyWiWxmc
// Time: 4:00

#include "Model.h"

// Proceduraly draw windows
const unsigned int windowWidth = 800;
const unsigned int windowHeight = 800;

float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
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

	Shader meshShader("default.vert", "explosion.geom", "default.frag");
	// Effect: show normals of triangles
	Shader normalsShader("default.vert", "normals.geom", "normals.frag");
	Shader skyboxShader("skybox.vert", "skybox.frag");

	glm::vec4 lightColor = glm::vec4(1, 1, 1,	 1);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	meshShader.Activate();
	glUniform4f(glGetUniformLocation(meshShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(meshShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	skyboxShader.Activate();
	// 0-th texture unit
	glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 0);


#pragma endregion

#pragma region RENDERING AND WINDOWHANDLING

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load in models
	Model model("models/statue/scene.gltf");

	// VAO is a wrapper
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	// Vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// Unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Directory of textures we want to use
	std::string facesCubemap[6] =
	{
		"Textures/Skybox/right.jpg",
		"Textures/Skybox/left.jpg",
		"Textures/Skybox/top.jpg",
		"Textures/Skybox/bottom.jpg",
		"Textures/Skybox/front.jpg",
		"Textures/Skybox/back.jpg"
	};

	// ID for the generated cubemap
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	// Bind the texture ID as a cubemap (that is what we will store in it!)
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Texture dimensions: XYZ -> STR
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // CLAMP EDGES!!!
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (unsigned int i = 0; i < 6; ++i)
	{
		int width, height, nrChannels;
		// Desiredchannels = 0 -> automatic
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			// Cubemaps start at the top left corner so we dont have to flip it!
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				/* Order of sides:
				  POSITIVE_X = right
				  NEGATIVE_X = left
				  POSITIVE_Y = top
				  NEGATIVE_Y = bottom
				  POSITIVE_Z = front	<- positive is the front face instead of the back face! we have to match it with our coordinate system later! (in shaders)
				  NEGATIVE_Z = back*/
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, /*We set here witch side are we talking about*/
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Falied to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	// FPS COUNTER
	double prevTime = 0.0;
	double crntTime = 0.0;
	double timeDiff;
	// How many frames in a certain amount of time
	unsigned int frameCounter = 0;
	const double frameMeasurementPerSecond = 60;

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

		// Tell GL to clear back buffer with specific color
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clears COLOR_BUFFER with the previously set clearColor
		// Clear depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Camera inputs: moving, rotating...
		camera.Inputs(window);
		// updates the cameraMatrix
		camera.updateMatrix(45.0f, 0.1f, 100.0f);

		// We want to sync the time with the model
		model.syncTime = true;
		// DRAW
		model.Draw(meshShader, camera);
		model.Draw(normalsShader, camera); // Show visually the direction of normals of the mesh

		// DRAW SKYBOX
		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		skyboxShader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We scale it back to mat3 then up to mat4, so it doesn't translate anything (4th row and col is 0s)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
		projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		// Cubemap
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0); // We are storing to TEXTURE0 in memory whatever we bind here
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, 3 * 2 * 6, GL_UNSIGNED_INT, 0); // We only have one texture, that is the 0-th in the unit
		glBindVertexArray(0); // Unbind cubemap
		
		glDepthFunc(GL_LESS); // Reset depth buffer func


		// Swaps the front and back buffers
		glfwSwapBuffers(window);

		// Processes basic events that we don't want to e.g.: Window close, Window resize, Move window
		glfwPollEvents();
	}
#pragma endregion

#pragma region DISMANTLE USED OBJECTS
	meshShader.Delete();
	skyboxShader.Delete();

	// Destroy the window after we finished using it
	glfwDestroyWindow(window);
	// Terminate graphics librari
	glfwTerminate();
#pragma endregion

	return 0;
}
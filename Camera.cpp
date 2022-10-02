#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// (Honnan nézek, hova nézek, FEL)
	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);

	// (location, matrixArray? moreThanOne : 1, load transpose?, pointer)
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::Inputs(GLFWwindow* window)
{
	// WASD
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}


	// UP DOWN
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		Position += speed * -Up;
	}


	// SPEED
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.1f;
	}

	// TURN
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// Hide cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// If we click on the window for the first time, the camera will rotate instantly in the direction of the cursor
		// If we don't want the camera to "jump", we can just simply reset the mouse pos to the middle
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2.), (height / 2.));
			firstClick = false;
		}

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2.0f)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2.0f)) / width;

		// Vertical rotation
		glm::vec3 newOrientationX = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Ha nagyon közel vagyunk Up-hoz vagy -Up-hoz akkor vigyáznunk kell nehogy túlforduljunk
		if (!(glm::angle(newOrientationX, Up) <= glm::radians(5.0f) or glm::angle(newOrientationX, -Up) <= glm::radians(5.0f)))
		{
			Orientation = newOrientationX;
		}

		// Horizontal rotation
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Set the cursor to the center of the screen, so it can't wander off
		glfwSetCursorPos(window, (width / 2.), (height / 2.));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Show cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// We left the window
		// We don't want to jump, so we remember this
		firstClick = true;
	}
}
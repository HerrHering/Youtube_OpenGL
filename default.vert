#version 330 core

// The data from the buffers will be read into these layouts
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// Output color to the next shader in the pipeline (fragment shader)
out vec3 color;

// Uniform value inside vertex shader
// Can be accessed directly from CPU
uniform float scale;

void main()
{
	vec3 scaledPos = aPos * scale;
	gl_Position = vec4(scaledPos.x, scaledPos.y, scaledPos.z, 1.0);
	color = aColor;
}
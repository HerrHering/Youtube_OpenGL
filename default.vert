#version 330 core

// The data from the buffers will be read into these layouts
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

// Output color to the next shader in the pipeline (fragment shader)
out vec3 color;

// UV
out vec2 texCoord;

// Uniform value inside vertex shader
// Can be accessed directly from CPU
uniform float scale;

void main()
{
	vec3 scaledPos = aPos * scale;
	gl_Position = vec4(scaledPos.x, scaledPos.y, scaledPos.z, 1.0);
	color = aColor;
	texCoord = aTex;
}
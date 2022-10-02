#version 330 core

// The data from the buffers will be read into these layouts
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

// Output color to the next shader in the pipeline (fragment shader)
out vec3 color;

// UV
out vec2 texCoord;

uniform mat4 camMatrix;

void main()
{
	gl_Position = camMatrix * vec4(aPos, 1.0f);
	color = aColor;
	texCoord = aTex;
}
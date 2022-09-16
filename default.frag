#version 330 core

// Outputs the final color to the "screen"
// NAME MATTERS
out vec4 FragColor;

// Requests a vec3 value called "color" from the previous shader in the pipeline (vertex shader)
in vec3 color;

void main()
{
	// Set the uotput value of the fragment shader
	FragColor = vec4(color, 1.0);
}
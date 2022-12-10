#version 330 core

// The data from the buffers will be read into these layouts
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

/*
	We replace thiese outputs with our custom datastructure

	out vec3 crntPos;
	out vec3 Normal;
	out vec3 color;
	out vec2 texCoord;
*/
out DATA // Custom datastructure
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
} data_out;

uniform mat4 camMatrix;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	// Set the data that we want to use in the !Geometry Shader!
	// Default datastructure -------------------------------
	gl_Position = model * translation * rotation * scale * vec4(aPos, 1.0f);
	// -----------------------------------------------------
	// Custom datastructure --------------------------------
	data_out.Normal = aNormal;
	data_out.color = aColor;
	data_out.texCoord = mat2(0, -1, 1, 0) * aTex; // Rotate texture coordinates by 90degrees FOR GOD KNWOS WHY
	data_out.projection = camMatrix;
	// -----------------------------------------------------
}
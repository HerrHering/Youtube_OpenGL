#version 330 core

// The data from the buffers will be read into these layouts
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

uniform mat4 camMatrix;

uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	// negative rotation 'couse glm and opengl uses clokwise and counterclockwise rot directions
	// crntPos = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));

	// Dont have to invert rotation direction
	crntPos = vec3(model * translation * rotation * scale * vec4(aPos, 1.0f));
	Normal = aNormal;
	color = aColor;
	// Rotate texture coordinates by 90degrees FOR GOD KNWOS WHY
	texCoord = mat2(0, -1, 1, 0) * aTex;

	gl_Position = camMatrix * vec4(crntPos, 1.0f);
}
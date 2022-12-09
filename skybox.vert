#version 330 core

layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	vec4 pos = projection * view * vec4(aPos, 1.0f);

	// Having z equal w will always result in a depth of 1.0f
	// The depth 1 means it is behind everything
	gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);

	// The positive z direction is in the opposing direction as OpenGL's!
	texCoords = vec3(aPos.x, aPos.y, -aPos.z);
}
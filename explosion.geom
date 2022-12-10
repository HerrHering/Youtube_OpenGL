#version 330 core

// Input layout:
//		We want to receive a "triangles" and export a "triangle_strip"
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Output !To Fragment Shader!
out vec3 Normal;
out vec3 color;
out vec2 texCoord;

in DATA
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
} data_in[];

// Just for fun - purposes
uniform float _Time;

// Assemble the geometry from the data we got FROM the !Fragment Shader!
void main()
{
	vec3 vector0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 vector1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	// Perpendicular vector to the triangle's surface
	vec4 surfaceNormal = vec4(normalize(cross(vector0, vector1)), 0.0f);
	// By displacing the triangles outwards, it looks like it is exploding! :)
	// Lets move the triangles in and out so it looks cool
	surfaceNormal *= (1 + sin(_Time)) * 0.1f;

	// First vertex
	gl_Position = data_in[0].projection * (gl_in[0].gl_Position + surfaceNormal);
	Normal = data_in[0].Normal;
	color = data_in[0].color;
	texCoord = data_in[0].texCoord;
	EmitVertex(); // With this command we say: "The first vertex's data has been assembled!"

	// Second vertex
	gl_Position = data_in[1].projection * (gl_in[1].gl_Position + surfaceNormal);
	Normal = data_in[1].Normal;
	color = data_in[1].color;
	texCoord = data_in[1].texCoord;
	EmitVertex();

	// Third vertex
	gl_Position = data_in[2].projection * (gl_in[2].gl_Position + surfaceNormal);
	Normal = data_in[2].Normal;
	color = data_in[2].color;
	texCoord = data_in[2].texCoord;
	EmitVertex();

	EndPrimitive(); // With this command we say: "The primitive has been assembled, all of its vertices are finished!";
}
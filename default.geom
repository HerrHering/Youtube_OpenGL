#version 330 core

/*
	Input primitive types:
		points
		lines
		lines_adjacency
		triangles
		triangles_adjacency

	Output primitive types:
		points
		line_strip
		triangle_strip
*/

// Input layout:
//		We want to receive a "triangles" and export a "triangle_strip"
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Output !To Fragment Shader!
out vec3 crntPos;
out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec4 fragPosLight;

//	Getting input data: !From Vertex Shader!
/*
	Default input structure: (we already have it built in!)

	in gl_PerVertex
	{
		vec4 gl_Position;
		float gl_PointSize;
		float gl_ClipDistance;
	} gl_in[];
*/
//	Custom extra input !From Vertex Shader!
in DATA // The name of the strucure, this could be anything
{
	vec3 crntPos;
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
	vec4 fragPosLight;
} data_in[]; // Maybe we are not working with only one vertex! -> input ARRAY (3 elements for a triangle)

// Assemble the geometry from the data we got FROM the !Fragment Shader!
void main()
{
	// Output values for the !Fragment Shader!
	// There are 3vertices that we need to construct (because a triangle has 3 vertices...)

	// First vertex
	gl_Position = data_in[0].projection * gl_in[0].gl_Position;
	crntPos = data_in[0].crntPos;
	Normal = data_in[0].Normal;
	color = data_in[0].color;
	texCoord = data_in[0].texCoord;
	fragPosLight = data_in[0].fragPosLight;
	EmitVertex(); // With this command we say: "The first vertex's data has been assembled!"

	// Second vertex
	gl_Position = data_in[1].projection * gl_in[1].gl_Position;
	crntPos = data_in[1].crntPos;
	Normal = data_in[1].Normal;
	color = data_in[1].color;
	texCoord = data_in[1].texCoord;
	fragPosLight = data_in[1].fragPosLight;
	EmitVertex();

	// Third vertex
	gl_Position = data_in[2].projection * gl_in[2].gl_Position;
	crntPos = data_in[2].crntPos;
	Normal = data_in[2].Normal;
	color = data_in[2].color;
	texCoord = data_in[2].texCoord;
	fragPosLight = data_in[2].fragPosLight;
	EmitVertex();

	EndPrimitive(); // With this command we say: "The primitive has been assembled, all of its vertices are finished!";
}
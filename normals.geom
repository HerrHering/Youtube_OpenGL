#version 330 core

layout (triangles) in;
// Output 3 lines per triangle, one for each triangle vertex (3 seqments -> 3 * 2 vertices: 1for end, 1for start)
layout (line_strip, max_vertices = 6) out;

in DATA
{
	vec3 Normal;
	vec3 color;
	vec2 texCoord;
	mat4 projection;
} data_in[];

void main()
{
	//		First line seqment
	// Start vertex
	gl_Position = data_in[0].projection * gl_in[0].gl_Position;
	EmitVertex();
	// End vertex
	gl_Position = data_in[0].projection * (gl_in[0].gl_Position + 0.01f * vec4(data_in[0].Normal, 0.0f));
	EmitVertex();
	EndPrimitive(); // The first line segment has been created

	//		Second line seqment
	// Start vertex
	gl_Position = data_in[1].projection * gl_in[1].gl_Position;
	EmitVertex();
	// End vertex
	gl_Position = data_in[1].projection * (gl_in[1].gl_Position + 0.01f * vec4(data_in[1].Normal, 1.0f));
	EmitVertex();
	EndPrimitive(); // The second line segment has been created

	//		Third line seqment
	// Start vertex
	gl_Position = data_in[2].projection * gl_in[2].gl_Position;
	EmitVertex();
	// End vertex
	gl_Position = data_in[2].projection * (gl_in[2].gl_Position + 0.01f * vec4(data_in[2].Normal, 2.0f));
	EmitVertex();
	EndPrimitive(); // The third line segment has been created


	// All 3 segments are completed
}
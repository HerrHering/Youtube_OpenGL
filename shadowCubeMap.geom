#version 330 core

layout (triangles) in;
// We input and output 6 triangles, one for each face
layout (triangle_strip, max_vertices = 18) out;

// Tansformation matrix for each side (projection)
uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main()
{
	// why do we only draw 1 triangle per face???
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; //  select a specific layer (or face and layer of cube map) in a multi-layer framebuffer attachment
		for (int i = 0; i < 3; ++i)
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = shadowMatrices[face] * FragPos;
			EmitVertex();
		}
		EndPrimitive();
	}
}
#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 Normal;
out vec3 color;
out vec2 texCoord;
out vec3 crntPos;
out vec3 lightPos;
out vec3 camPos;

in DATA
{
    vec3 Normal;
	vec3 color;
	vec2 texCoord;
    mat4 projection;
    mat4 model;
    vec3 lightPos;
	vec3 camPos;
} data_in[];

// Default main function
void main()
{
    // Problem:
    //      The normal map is always facing into the positive z direction, and if the surface isn't oriented that way it doesn't work...
    //      We want the normalmap to face to the right way
    // Solution:
    //      Use TBN matrix, to transform the normalmap space!
    //      (so it isn't always facing to the positive z direction, but the direction of the triangle instead!)

    // CRAZY MAGIC MATRIX FUCKERY
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    // https://ogldev.org/www/tutorial26/tutorial26.html
    
    // We need all three vertices of a triangle to calculate the matrix so we can either do it here, on on the CPU
    // but the CPU is slower, so we do it in geometry shader

    // Edges of the triangle
    vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    // Lengths of UV differences
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;

    // one over the determinant
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    mat3 TBN = mat3(T, B, N);
    // TBN is an orthogonal matrix and so its inverse is equal to its transpose
    TBN = transpose(TBN);

    // We need the light variables and the normal map to be in the same space
    // (now they are in different spaces bc. the normal map faces always positive Z direction, and the light changes triangle by tirangle)
    // Option 1
    //      Get the normal map to the same place as the light variables
    //          -> Only can be done in fragment shader -> will be called a lot more times then the geometry shader -> SLOW :(
    // Option 2
    //      Get the other light variables into the same space as the normal map
    //          -> also can be called in geometry shader -> MUCH fewer calls -> SPEED :)


    gl_Position = data_in[0].projection * gl_in[0].gl_Position;
    Normal = data_in[0].Normal;
    color = data_in[0].color;
    texCoord = data_in[0].texCoord;
    // Change all lighting variables to TBN space
    crntPos = TBN * gl_in[0].gl_Position.xyz;
    lightPos = TBN * data_in[0].lightPos;
    camPos = TBN * data_in[0].camPos;
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    Normal = data_in[1].Normal;
    color = data_in[1].color;
    texCoord = data_in[1].texCoord;
    // Change all lighting variables to TBN space
    crntPos = TBN * gl_in[1].gl_Position.xyz;
    lightPos = TBN * data_in[1].lightPos;
    camPos = TBN * data_in[1].camPos;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    Normal = data_in[2].Normal;
    color = data_in[2].color;
    texCoord = data_in[2].texCoord;
    // Change all lighting variables to TBN space
    crntPos = TBN * gl_in[2].gl_Position.xyz;
    lightPos = TBN * data_in[2].lightPos;
    camPos = TBN * data_in[2].camPos;
    EmitVertex();

    EndPrimitive();
}
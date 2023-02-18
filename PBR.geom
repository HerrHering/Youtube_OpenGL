#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

// Outputs for the fragment shader
out vec3 _Normal;
out vec3 _color;
out vec2 _texCoord;
out vec3 _crntPos;
out vec3 _lightPos;
out vec3 _camPos;

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
    vec2 deltaUV0 = data_in[1].texCoord - data_in[0].texCoord;
    vec2 deltaUV1 = data_in[2].texCoord - data_in[0].texCoord;
    // Lengths of UV differences: The deltaU components are parallel to the T vector and the deltaV components are parallel to the B vector
    // Edge = dU * T + dV * B

    // We now have a system of linear equations:
    //  (E0x, E0y, E0z) = dU0 * (Tx, Ty, Tz) + dV0 * (Bx, By, Bz)
    //  (E1x, E1y, E1z) = dU1 * (Tx, Ty, Tz) + dV1 * (Bx, By, Bz)
    // This can be turned into a matrix equation:
    //      [E0x, E0y, E0z] = [dU0, dV0] * [Tx, Ty, Tz]
    //      [E1x, E1y, E1z] = [dU1, dV1] * [Bx, By, Bz]
    //  -> E = dUV * TB -> TB = inv(dUV) * E -> Calculations:

    // one over the determinant of the dUV matrix2x2
    float invDet = 1.0f / (deltaUV0.x * deltaUV1.y - deltaUV1.x * deltaUV0.y);

    vec3 tangent = vec3(invDet * (deltaUV1.y * edge0 - deltaUV0.y * edge1));
    // We will calculate the bitangent vector when reorthogonalizing T, we don't want to calculate it twice :)
    //vec3 bitangent = vec3(invDet * (-deltaUV1.x * edge0 + deltaUV0.x * edge1));

    // Bring the tangent and bitangent vectors to global space with model matrix and,
    // Retrieve the T and B vectors from the matrix equation's solution row-by-row:


    vec3 T = normalize(vec3(data_in[0].model * vec4(tangent, 0.0f)));
    // We will calculate B when reorthogonalizing T, we don't want to calculate it twice :)
    //vec3 B = normalize(vec3(data_in[0].model * vec4(bitangent, 0.0f)));
    // Normal of the current triangle: cross product of edges
    vec3 N = normalize(vec3(data_in[0].model * vec4(cross(edge1, edge0), 0.0f)));

    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
    // Construct the matrix
    mat3 TBN = mat3(T, B, N);
    // TBN is an ORTHOGONAL matrix (we made it sure by reorthogonalizing the tangent vector) and so its inverse is equal to its transpose
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
    _Normal = data_in[0].Normal;
    _color = data_in[0].color;
    _texCoord = data_in[0].texCoord;
    // Change all lighting variables to TBN space
    _crntPos = TBN * gl_in[0].gl_Position.xyz;
    _lightPos = TBN * data_in[0].lightPos;
    _camPos = TBN * data_in[0].camPos;
    EmitVertex();

    gl_Position = data_in[1].projection * gl_in[1].gl_Position;
    _Normal = data_in[1].Normal;
    _color = data_in[1].color;
    _texCoord = data_in[1].texCoord;
    // Change all lighting variables to TBN space
    _crntPos = TBN * gl_in[1].gl_Position.xyz;
    _lightPos = TBN * data_in[1].lightPos;
    _camPos = TBN * data_in[1].camPos;
    EmitVertex();

    gl_Position = data_in[2].projection * gl_in[2].gl_Position;
    _Normal = data_in[2].Normal;
    _color = data_in[2].color;
    _texCoord = data_in[2].texCoord;
    // Change all lighting variables to TBN space
    _crntPos = TBN * gl_in[2].gl_Position.xyz;
    _lightPos = TBN * data_in[2].lightPos;
    _camPos = TBN * data_in[2].camPos;
    EmitVertex();

    EndPrimitive();
}
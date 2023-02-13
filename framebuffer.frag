#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma;


void main()
{
    vec3 fragment = texture(screenTexture, texCoords).rgb;
    
    // Tone mapping
    // How much light do we want to get into the camera
    // e.g. in underground areas we need more light to see -> high exposure
    //      outside we need less light (bc there is the sun, and we dont wanna blind) -> low exposure
    float exposure = 0.1f;
    // HDR range: [0, inf] -> Monitor range: [0, 1]
    vec3 toneMapped = vec3(1.0f) - exp(-fragment * exposure);

    // When we are using SRGB textures, we have to gamma correct, so when the rendering is done and the gamma is applied everything is linear!
    FragColor.rgb = pow(toneMapped, vec3(1.0f / gamma));
}
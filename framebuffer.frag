#version 330 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;
uniform float gamma;


void main()
{
    vec4 fragment = texture(screenTexture, texCoords);
    
    // When we are using SRGB textures, we have to gamma correct, so when the rendering is done and the gamma is applied everything is linear!
    FragColor.rgb = pow(fragment.rgb, vec3(1.0f / gamma));
}
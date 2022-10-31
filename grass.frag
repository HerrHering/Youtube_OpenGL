#version 330 core

// Outputs the final color to the "screen"
// NAME MATTERS
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 color;
in vec2 texCoord;

// Input texture

// Material texture
uniform sampler2D diffuse0;
// Specular map
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

vec4 DirecLight()
{
	// HARDCODED
	vec3 _lightDir = normalize(vec3(1, 1, 0));

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = _lightDir;
	float diffuse = max(0, abs(dot(normal, lightDirection))); // abs helps with grass shading

	// Max intensity
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	// The power tells us how pointy the reflection is (higher power -> smaller point)
	float specAmount = pow(max(0, dot(viewDirection, reflectionDirection)), 16);
	float specular = specularLight * specAmount * texture(specular0, texCoord).r;

	// if something is transparent we dont draw it
	if (texture(diffuse0, texCoord).a < 0.1f)
	{
		discard;
	}

	return (texture(diffuse0, texCoord) * (ambient + diffuse) + specular) * lightColor;
}

float near = 0.1f;
float far = 100.0f;

// Originally the depth value is between 0 and 1, and is not linear (so we dont have to use high precision far away, where we cant see)
// This will return the distance of the fragment from the near plane
float linearizeDepth(float depth)
{
	return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

// steepness: how fase are we going from 0 to 1
// offset: when are we halfway done (when is our depth value 50%)
float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
	float zVal = linearizeDepth(depth);

	return (1.0 / (1.0 + exp(-steepness * (zVal - offset))));
}

void main()
{
	// Normalized, logistic
	float depth = logisticDepth(gl_FragCoord.z, 0.3f, 20f);
	// The far away object blend into the backround wrp to the distance
	// Interpolation between background and light color:
	FragColor = DirecLight() * (1.0 - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}
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

vec4 PointLight()
{
	vec3 lightVec = lightPos - crntPos;
	float dist = length(lightVec);
	float a = 1.0f; // Light reach
	float b = 0.04f; // How fast light dies off
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(0, dot(normal, lightDirection));

	// Max intensity
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	// The power tells us how pointy the reflection is (higher power -> smaller point)
	float specAmount = pow(max(0, dot(viewDirection, reflectionDirection)), 16);
	float specular = specularLight * specAmount * texture(specular0, texCoord).r;

	return (texture(diffuse0, texCoord) * (ambient + diffuse * inten) + specular * inten) * lightColor;
}

vec4 DirecLight()
{
	// HARDCODED
	vec3 _lightDir = normalize(vec3(1, 1, 0));

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = _lightDir;
	float diffuse = max(0, dot(normal, lightDirection));

	// Max intensity
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	// The power tells us how pointy the reflection is (higher power -> smaller point)
	float specAmount = pow(max(0, dot(viewDirection, reflectionDirection)), 16);
	float specular = specularLight * specAmount * texture(specular0, texCoord).r;

	return (texture(diffuse0, texCoord) * (ambient + diffuse) + specular) * lightColor;
}

vec4 SpotLight()
{
	// angle in radians between the light dir and the side of the cone
	float outerCone = 0.9f;
	float innerCone = 0.95f;

	float ambient = 0.2f;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(0, dot(normal, lightDirection));

	// Max intensity
	float specularLight = 0.5f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	// The power tells us how pointy the reflection is (higher power -> smaller point)
	float specAmount = pow(max(0, dot(viewDirection, reflectionDirection)), 16);
	float specular = specularLight * specAmount * texture(specular0, texCoord).r;

	// The cosine of the angle between the light direction and crntPos
	float angle = dot(vec3(0, -1, 0), -lightDirection);
	// Inverselerp of the angle between the 2 view cones
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0, 1);

	return (texture(diffuse0, texCoord) * (ambient + diffuse * inten) + specular * inten) * lightColor;
}

void main()
{
	

	// Set the uotput value of the fragment shader
	//FragColor = SpotLight();
	FragColor = DirecLight();
	//FragColor = PointLight();
}
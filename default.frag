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

// Phong lighting
vec4 PointLight_Phong()
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

// Blinn-Phong lighting:
//		The cosine part of dot product(view*light) can go to negative and thus the specular light will be cut off :(
//	Solution: Dotproduct between the normal and the HALFWAY vector -> cosine never can be negative -> no cutoff :)
vec4 PointLight_BlinnPhong()
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

	// DIFFERENCE
	// Vector halfway between the view direction(point->camera) and the reflected light direction(point->reflectiondir.)
	vec3 halfwayVec = normalize(viewDirection + lightDirection);

	// The power tells us how pointy the reflection is (higher power -> smaller point)
	//float specAmount = pow(max(0, dot(viewDirection, reflectionDirection)), 16);
	float specAmount = pow(max(0, dot(normal, halfwayVec)), 16); // Corrigate reflection->no cutoff
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
	// Set the output value of the fragment shader
	
	// LIGHTING MODELS
	//FragColor = SpotLight();
	FragColor = DirecLight();
	//FragColor = PointLight();

	// TEST DEPTHBUFFER

	// Normalized, linear distance to the near plane
	// float fragDstNear = linearizeDepth(gl_FragCoord.z) / far;
	// FragColor = vec4(vec3(fragDstNear), 1.0f);

//	// Normalized, logistic
//	float depth = logisticDepth(gl_FragCoord.z, 0.2f, 20f);
//	// The far away object blend into the backround wrp to the distance
//	// Interpolation between background and light color:
//	FragColor = DirecLight() * (1.0 - depth) + vec4(depth * vec3(0.85f, 0.85f, 0.90f), 1.0f);
}
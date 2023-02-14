#version 330 core

// Outputs colors in RGBA
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

// Imports the current position from the Vertex Shader
in vec3 crntPos;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the color from the Vertex Shader
in vec3 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Gets the position of the light from the main function
in vec3 lightPos;
// Gets the position of the camera from the main function
in vec3 camPos;


// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;
// Gets the color of the light from the main function
uniform vec4 lightColor;



vec4 pointLight()
{	
	// used in two variables so I calculate it here to not have to do it twice
	vec3 lightVec = lightPos - crntPos;

	// intensity of light with respect to distance
	float dist = length(lightVec);
	float a = 1.00f;
	float b = 0.70f;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.05f;

	// https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
	vec3 viewDirection = normalize(camPos - crntPos);
	// Occlusion map settings
	float heightScale = 0.05f; // Max depth of illusion
	// We will make small steps forward (from layer to layer) untill we get below the surface, then we stop
	const float minLayers = 8.0f; // Min amount of layers to check (before this, we wont stop)
	const float maxLayers = 64.0f; // Max layers to check (after this we will stop sampling steps, and suppose we are at the bottom)
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 0, 1), viewDirection))); // The steeper we look, the further down we have to go (just approximations)
	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f; // How much did we go into the surface

	// Dividint by z is optinal, it will make the depth look more dramatic
	// After we applied the TBN matrix, we are looking perpendicular to the surface, so the xy components discribe where we are looking at
	vec2 S = viewDirection.xy / viewDirection.z * heightScale;
	// The more we go "into" the texture, the closer we will be to the center, because of perspective stuff (excepth when we are looking at the center from above)
	// This parameter tells us: "how much will we be off" from "going into the surface"
	vec2 deltaUVs = S / numLayers; // We will get closer layer-by-layer

	vec2 UVs = texCoord;
	float currentDepthMapValue = 1.0f - texture(displacement0, UVs).r;

	// Loop till we are above the "depth surface", then stop, because we hit the surface
	// If the "next" layer is below surface we stop
	while (currentLayerDepth < currentDepthMapValue)
	{
		UVs -= deltaUVs; // Because of perspective stuff, we will get closer to the center by going further into the surface (see abouve definition)
		currentDepthMapValue = 1.0f - texture(displacement0, UVs).r;
		currentLayerDepth += layerDepth; // We go to the next layer
	}

	// Apply occlusion (the faked depth)
	// We are getting closer to the center layer-by-layer, this parameter is the UV before we went below sufrace
	vec2 prevTexCoords = UVs + deltaUVs;
	// We are slightly below the surface because we immediately stopped when we stepped below it
	// Calculate the distance between the surface and the "slightly below" depth point
	float afterDepth = currentDepthMapValue - currentLayerDepth;

	// WHAT THE ECTUAL FUCK IS GOING ON HERE????? https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
	float beforeDepth = (1.0f - texture(displacement0, prevTexCoords).r) - currentDepthMapValue + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	UVs = prevTexCoords * weight + UVs * (1.0f - weight);

	// Get rid of everything outside the normal range
	if (UVs.y > 1.0f || UVs.x > 1.0f || UVs.y < 0.0 || UVs.x < 0.0f)
		discard;

	// diffuse lighting
	// Normals are mapped from the range [0, 1] to the range [-1, 1]
	vec3 normal = normalize(texture(normal0, UVs).xyz * 2.0f - 1.0f);
	vec3 lightDirection = normalize(lightVec);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	return (texture(diffuse0, UVs) * (diffuse * inten + ambient) + texture(specular0, UVs).r * specular * inten) * lightColor;
}

vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	return (texture(diffuse0, texCoord) * (diffuse + ambient) + texture(specular0, texCoord).r * specular) * lightColor;
}

vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specular = 0.0f;
	if (diffuse != 0.0f)
	{
		float specularLight = 0.50f;
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 halfwayVec = normalize(viewDirection + lightDirection);
		float specAmount = pow(max(dot(normal, halfwayVec), 0.0f), 16);
		specular = specAmount * specularLight;
	};

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texCoord) * (diffuse * inten + ambient) + texture(specular0, texCoord).r * specular * inten) * lightColor;
}


void main()
{
	// outputs final color
	FragColor = pointLight();

	// Highlight the red parts, because in the image we are using red=lava -> bright
	if (FragColor.r > 0.05f)
		FragColor.r *= 5.0f;

	// The brightness of the current pixel (grayscaled value)
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	// If it is "bright enough", it will shine! (= bloom!)
	if (brightness > 0.15f)
		BloomColor = vec4(FragColor.rgb, 1.0f);
	else
		BloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
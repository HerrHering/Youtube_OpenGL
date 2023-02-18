#version 330 core

// Outputs colors in RGBA
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BloomColor;

// Imports the current position from the Geometry Shader
in vec3 _crntPos;
in vec3 _Normal;
in vec3 _color;
in vec2 _texCoord;
in vec3 _lightPos;
in vec3 _camPos;

#define PI 3.141592
#define MINDIV 0.0001

// Gets the Texture Units from the main function
uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler2D normal0;
uniform sampler2D displacement0;
// Gets the color of the light from the main function
uniform vec3 lightColor;
uniform vec3 meshEmissivity;
uniform float roughness; // Bumpy surface
uniform vec3 baseReflectance;
uniform float metallic;

// GGX/Trowbridge-Reitz Normal Distribution Function
float D(float alpha, vec3 N, vec3 H)
{
	float alpha2 = alpha * alpha;
	float numerator = alpha2;

	float NdotH = max(dot(N, H), 0.0);
	float base = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
	float denominator = PI * base * base;

	denominator = max(denominator, MINDIV);
	return numerator / denominator;
}

// Schlick-Beckmann Geometry Shadowing Function
float G1(float alpha, vec3 N, vec3 X)
{
	float numerator = max(dot(N, X), 0.0);

	float k = alpha * 0.5;
	float denominator = numerator * (1.0 - k) + k;

	denominator = max(denominator, MINDIV);
	return numerator / denominator;
}

// Smith model
float G(float alpha, vec3 N, vec3 V, vec3 L)
{
	// The camera would normally be able to see these points, but we are creating "fake bumpyness" for more realistic looks
	//	Shadow no.1: The surface is bumpy, so some parts are not visible by the camera (the sufrace is flat, but the simulation calculation makes it bumpy)
	//	Shadow no.2: As the light bounces off the surface, not all of goes into out eyes, because of the bumpyness (again, the surface is flat, but the simulation calculation makes it bumpy)
	return G1(alpha, N, V) * G1(alpha, N, L);
}

// Fresnel-Schlick Function
vec3 F(vec3 F0, vec3 V, vec3 H)
{
	float base = 1 - max(dot(V, H), 0.0);
	return F0 + (vec3(1.0f) - F0) * base * base * base * base * base;
}

vec2 CalcUVs(vec3 viewDirection)
{
	// PARALLAX MAPPING

	// https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
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

	vec2 UVs = _texCoord;
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

	// WHAT THE ACTUAL FUCK IS GOING ON HERE????? https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
	float beforeDepth = (1.0f - texture(displacement0, prevTexCoords).r) - currentDepthMapValue + layerDepth;
	float weight = afterDepth / (afterDepth - beforeDepth);
	UVs = prevTexCoords * weight + UVs * (1.0f - weight);

	// Get rid of everything outside the normal range
	if (UVs.y > 1.0f || UVs.x > 1.0f || UVs.y < 0.0 || UVs.x < 0.0f)
		discard;

	// END: PARRALAX MAPPING
	return UVs;
}


// https://www.youtube.com/watch?v=RRE-F57fbXw&list=PL-TZKGUEmXltQ6vJhJUICmu1TyIrygHlO&index=3&ab_channel=VictorGordan
vec3 pointLight()
{
	// used in two variables so I calculate it here to not have to do it twice
	vec3 viewDirection = normalize(_camPos - _crntPos);
	vec3 lightVec = normalize(_lightPos - _crntPos);
	vec3 halfwayVec = normalize(lightVec + viewDirection);

	// Calculate the uvs of the texture
	vec2 UVs = CalcUVs(viewDirection);

	// intensity of light with respect to distance
	float dist = length(_lightPos - _crntPos);
	float a = 1.00f;
	float b = 0.70f;
	float inten = 1.0f / (a * dist * dist + b * dist + 1.0f);

	// ambient lighting
	float ambient = 0.05f;

	// texCoords has been updated to UVs!
	// Read from textures
	vec3 t_normal = normalize(texture(normal0, UVs).xyz * 2.0f - 1.0f);

	// PBR
	// Specular light ratio
	vec3 Ks = F(baseReflectance, viewDirection, halfwayVec);
	//vec3 Ks = F(vec3(1.0) - texture(diffuse0, UVs).rgb, viewDirection, halfwayVec); // LAZY SOLUTION
	// Diffuse light ratio
	vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metallic);

	vec3 lambert = texture(diffuse0, UVs).rgb / PI;

	float cookTorranceNumerator = D(roughness, t_normal, halfwayVec) * G(roughness, t_normal, viewDirection, lightVec);
	float cookTorranceDenominator = 4.0 * max(dot(viewDirection, t_normal), 0.0) * max(dot(lightVec, t_normal), 0.0);
	cookTorranceDenominator = max(cookTorranceDenominator, MINDIV);
	float cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

	vec3 BRDF = Kd * lambert + Ks * cookTorrance; // * texture(specular0, UVs).r;
	vec3 outgoingLight = meshEmissivity + BRDF * (lightColor * (inten + ambient)) * max(dot(lightVec, t_normal), 0.0);

	return outgoingLight;
}

void main()
{
	// outputs final color
	FragColor.rgb = pointLight();

	// Highlight the red parts, because in the image we are using red=lava -> bright
	if (FragColor.r > 0.05f)
		FragColor.r *= 5.0f;

	// The brightness of the current pixel (grayscaled value)
	float brightness = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
	// If it is "bright enough", it will shine! (= bloom!)
	if (brightness > 0.1f)
		BloomColor = vec4(FragColor.rgb, 1.0f);
	else
		BloomColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
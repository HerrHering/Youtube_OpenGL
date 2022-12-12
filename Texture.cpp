#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	type = texType;
	stbi_set_flip_vertically_on_load(true);

	int widthImg, heightImg, numColCh;
	// (filename, width, height, color channels, default/automatic color channels)
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// (count, ID)
	glGenTextures(1, &ID);
	// Assign texture to "texture unit" aka "texture boundle"
	// Activate texture unit
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	// What to do if image is scaled up or down
	// (type, setting, setting value)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// What to do if we get out of the 0:1 UV range
	// STR = xyz
	// (type, setting, setting value)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// (type, 0, texture color, width, height, 0, image color, pixel datatype, pixel data)
	// texture color : (how do we want to interpret the pixelvalues, in which colorspace)
	// RGB = Jpeg, RGBA = png
	// PNG
	// We want to interpret the pixel colors in the texture as SRGB color space values, so we can linearly calulate and gamma correct them
	if (numColCh == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	// JPEG
	else if (numColCh == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, widthImg, heightImg, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	// GRAYSCALED
	else if (numColCh == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	}
	else
		throw std::invalid_argument("Automatic Texture type recognition failed!");


	// Lower resolution copy of the original image
	glGenerateMipmap(GL_TEXTURE_2D);

	// Releasing resources
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Set Texture
	GLuint tex0Uni = glGetUniformLocation(shader.ID, uniform);
	shader.Activate();
	// 0-th texture in the boundle
	glUniform1i(tex0Uni, unit);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

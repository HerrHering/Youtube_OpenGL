#include "Texture.h"

Texture::Texture(const char* image, const char* texType, GLuint slot, GLenum format, GLenum pixelType)
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
	// RGB = Jpeg, RGBA = png
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
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

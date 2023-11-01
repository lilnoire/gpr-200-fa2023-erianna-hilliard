#include "texture.h"
#include "../eh/external/stb_image.h"
#include "../eh/external/glad.h"


static int textureFormat(int numComponents)
{
	switch (numComponents) 
	{
	default:
		return GL_RGBA;
	case 3:
		return GL_RGB;
	case 2:
		return GL_RG;
	}
}


unsigned int loadTexture(const char* filePath, int wrapMode, int filterMode)
{

	int width, height, numComponents = 4;

	unsigned char* data = stbi_load(filePath, &width, &height, &numComponents, 0);
	if (data == NULL)
	{
		printf("Failed to load image %s", filePath);
		stbi_image_free(data);
		return 0;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	int format = textureFormat(numComponents); 
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

	float borderColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

	glGenerateMipmap(GL_TEXTURE_2D); 

	glBindTexture(GL_TEXTURE_2D, NULL); 
	stbi_image_free(data);

	return texture;

}
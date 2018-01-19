#include "random_texture.h"
#include "util.h"


RandomTexture::RandomTexture()
{
	m_texture = 0;
}


RandomTexture::~RandomTexture()
{
	if (m_texture != 0)
	{
		glDeleteTextures(1, &m_texture);
	}
}


bool RandomTexture::Init(uint size)
{
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	const auto TextureSize = size * size * 3;
	const auto pRandom = new float[TextureSize];
	for (uint i = 0; i < TextureSize; i++)
	{
		const auto r = 2.0f * static_cast<float>(rand()) / RAND_MAX - 1.0f;
		pRandom[i] = r;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, pRandom);

	delete [] pRandom;

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return GLCheckError();
}

void RandomTexture::Bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

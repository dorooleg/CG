#pragma once

#include <GL/glew.h>

#include "types.h"

class RandomTexture
{
public:
	RandomTexture();

	bool Init(uint size);

	~RandomTexture();

	void Bind(GLenum TextureUnit);

private:
	uint m_texture;
};

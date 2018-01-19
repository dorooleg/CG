#pragma once

#include "technique.h"
#include "math_3d.h"

class BillboardTechnique : public Technique
{
public:

	BillboardTechnique();

	bool Init() override;

	void SetVP(const Matrix4f& VP) const;
	void SetCameraPosition(const Vector3f& Pos) const;
	void SetColorTextureUnit(unsigned int TextureUnit) const;
	void SetBillboardSize(float BillboardSize) const;

private:

	GLuint m_VPLocation;
	GLuint m_cameraPosLocation;
	GLuint m_colorMapLocation;
	GLuint m_billboardSizeLocation;
};

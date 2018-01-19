#include "billboard_technique.h"
#include "util.h"

BillboardTechnique::BillboardTechnique()
{
}

bool BillboardTechnique::Init()
{
	if (!Technique::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "billboard.vs"))
	{
		return false;
	}

	if (!AddShader(GL_GEOMETRY_SHADER, "billboard.gs"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "billboard.fs"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	m_VPLocation = GetUniformLocation("gVP");
	m_cameraPosLocation = GetUniformLocation("gCameraPos");
	m_colorMapLocation = GetUniformLocation("gColorMap");
	m_billboardSizeLocation = GetUniformLocation("gBillboardSize");

	if (m_VPLocation == INVALID_UNIFORM_LOCATION ||
		m_cameraPosLocation == INVALID_UNIFORM_LOCATION ||
		m_billboardSizeLocation == INVALID_UNIFORM_LOCATION ||
		m_colorMapLocation == INVALID_UNIFORM_LOCATION)
	{
		return false;
	}

	return GLCheckError();
}


void BillboardTechnique::SetVP(const Matrix4f& VP) const
{
	glUniformMatrix4fv(m_VPLocation, 1, GL_TRUE, (const GLfloat*)VP.m);
}


void BillboardTechnique::SetCameraPosition(const Vector3f& Pos) const
{
	glUniform3f(m_cameraPosLocation, Pos.x, Pos.y, Pos.z);
}


void BillboardTechnique::SetColorTextureUnit(unsigned int TextureUnit) const
{
	glUniform1i(m_colorMapLocation, TextureUnit);
}


void BillboardTechnique::SetBillboardSize(float BillboardSize) const
{
	glUniform1f(m_billboardSizeLocation, BillboardSize);
}

#include "util.h"
#include "ps_update_technique.h"

PSUpdateTechnique::PSUpdateTechnique()
{
}

bool PSUpdateTechnique::Init()
{
	if (!Technique::Init())
	{
		return false;
	}

	if (!AddShader(GL_VERTEX_SHADER, "ps_update.vs"))
	{
		return false;
	}


	if (!AddShader(GL_GEOMETRY_SHADER, "ps_update.gs"))
	{
		return false;
	}

	if (!AddShader(GL_FRAGMENT_SHADER, "ps_update.fs"))
	{
		return false;
	}

	if (!Finalize())
	{
		return false;
	}

	const GLchar* Varyings[3];
	Varyings[0] = "Type1";
	Varyings[1] = "Position1";
	Varyings[2] = "Velocity1";

	glTransformFeedbackVaryings(m_shaderProg, 3, Varyings, GL_INTERLEAVED_ATTRIBS);

	if (!Finalize())
	{
		return false;
	}


	m_randomTextureLocation = GetUniformLocation("gRandomTexture");
	m_timeLocation = GetUniformLocation("gTime");
	m_deltaTimeMillisLocation = GetUniformLocation("gDeltaTimeMillis");

	if (m_deltaTimeMillisLocation == INVALID_UNIFORM_LOCATION ||
		m_timeLocation == INVALID_UNIFORM_LOCATION ||
		m_randomTextureLocation == INVALID_UNIFORM_LOCATION)
	{
		return false;
	}

	return true;
}


void PSUpdateTechnique::SetDeltaTimeMillis(const unsigned int DeltaTimeMillis) const
{
	glUniform1f(m_deltaTimeMillisLocation, static_cast<float>(DeltaTimeMillis));
}


void PSUpdateTechnique::SetTime(int Time) const
{
	glUniform1f(m_timeLocation, static_cast<float>(Time));
}


void PSUpdateTechnique::SetRandomTextureUnit(unsigned int TextureUnit) const
{
	glUniform1i(m_randomTextureLocation, TextureUnit);
}

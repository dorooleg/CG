#pragma once

#include "technique.h"

class PSUpdateTechnique : public Technique
{
public:
	PSUpdateTechnique();

	bool Init() override;

	void SetParticleLifetime(float Lifetime);

	void SetDeltaTimeMillis(unsigned int DeltaTimeMillis) const;

	void SetTime(int Time) const;

	void SetRandomTextureUnit(unsigned int TextureUnit) const;

	void SetLauncherLifetime(float Lifetime);

	void SetShellLifetime(float Lifetime);

	void SetSecondaryShellLifetime(float Lifetime);

private:
	GLuint m_deltaTimeMillisLocation;
	GLuint m_randomTextureLocation;
	GLuint m_timeLocation;
};

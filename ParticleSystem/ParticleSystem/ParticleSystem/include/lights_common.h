#pragma once

#include "ATB/AntTweakBar.h"

#include "math_3d.h"

class BaseLight
{
public:
	string Name;
	Vector3f Color;
	float AmbientIntensity;
	float DiffuseIntensity;

	BaseLight()
	{
		Color = Vector3f(0.0f, 0.0f, 0.0f);
		AmbientIntensity = 0.0f;
		DiffuseIntensity = 0.0f;
	}

	virtual void AddToATB(TwBar* bar);
};


class DirectionalLight : public BaseLight
{
public:
	Vector3f Direction;

	DirectionalLight()
	{
		Direction = Vector3f(0.0f, 0.0f, 0.0f);
	}

	void AddToATB(TwBar* bar) override;
};


struct LightAttenuation
{
	float Constant;
	float Linear;
	float Exp;

	LightAttenuation()
	{
		Constant = 1.0f;
		Linear = 0.0f;
		Exp = 0.0f;
	}
};


class PointLight : public BaseLight
{
public:
	Vector3f Position;
	LightAttenuation Attenuation;

	PointLight()
	{
		Position = Vector3f(0.0f, 0.0f, 0.0f);
	}

	void AddToATB(TwBar* bar) override;
};


class SpotLight : public PointLight
{
public:
	Vector3f Direction;
	float Cutoff;

	SpotLight()
	{
		Direction = Vector3f(0.0f, 0.0f, 0.0f);
		Cutoff = 0.0f;
	}

	void AddToATB(TwBar* bar) override;
};


#define COLOR_WHITE Vector3f(1.0f, 1.0f, 1.0f)
#define COLOR_RED Vector3f(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN Vector3f(0.0f, 1.0f, 0.0f)
#define COLOR_CYAN Vector3f(0.0f, 1.0f, 1.0f)
#define COLOR_BLUE Vector3f(0.0f, 0.0f, 1.0f)

#pragma once

#include <shader_m.h>

class view
{
public:
	virtual ~view() = default;
	virtual void draw() = 0;
	virtual void set_shader(const Shader* shader) = 0;
};

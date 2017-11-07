#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <string>
#include <iostream>
#include <cerrno>

class wavefront
{
public:
	explicit wavefront(const std::string& path);

	const tinyobj::attrib_t& get_attributes() const;

	const std::vector<tinyobj::shape_t>& get_shapes() const;

private:
	static constexpr char* wavefront_error = "Wavefront error: ";
	tinyobj::attrib_t attrib_;
	std::vector<tinyobj::shape_t> shapes_;
	std::vector<tinyobj::material_t> materials_;
};

inline wavefront::wavefront(const std::string& path)
{
	std::string error;
	const auto result = LoadObj(&attrib_, &shapes_, &materials_, &error, path.c_str());

	if (!error.empty())
	{
		std::cerr << wavefront_error << error << std::endl;
	}

	if (!result)
	{
		std::cerr << wavefront_error << "Failed to load file = " << path << std::endl;
		exit(ENOTSUP);
	}

	if (shapes_.empty())
	{
		std::cerr << wavefront_error << "# shapes are zero" << std::endl;
		exit(ENOTSUP);
	}
}

inline const tinyobj::attrib_t& wavefront::get_attributes() const
{
	return attrib_;
}

inline const std::vector<tinyobj::shape_t>& wavefront::get_shapes() const
{
	return shapes_;
}

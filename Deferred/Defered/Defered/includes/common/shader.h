#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int id;

	Shader(const char* vertex_path, const char* fragment_path, const char* geometry_path = nullptr)
	{
		std::string vertex_code;
		std::string fragment_code;
		std::string geometry_code;
		std::ifstream v_shader_file;
		std::ifstream f_shader_file;
		std::ifstream g_shader_file;

		v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		g_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			v_shader_file.open(vertex_path);
			f_shader_file.open(fragment_path);
			std::stringstream v_shader_stream, f_shader_stream;

			v_shader_stream << v_shader_file.rdbuf();
			f_shader_stream << f_shader_file.rdbuf();

			v_shader_file.close();
			f_shader_file.close();

			vertex_code = v_shader_stream.str();
			fragment_code = f_shader_stream.str();

			if (geometry_path != nullptr)
			{
				g_shader_file.open(geometry_path);
				std::stringstream gShaderStream;
				gShaderStream << g_shader_file.rdbuf();
				g_shader_file.close();
				geometry_code = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ " << e.what() << std::endl;
		}
		auto v_shader_code = vertex_code.c_str();
		auto f_shader_code = fragment_code.c_str();

		const auto vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &v_shader_code, nullptr);
		glCompileShader(vertex);
		check_compile_errors(vertex, "VERTEX");

		const auto fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &f_shader_code, nullptr);
		glCompileShader(fragment);
		check_compile_errors(fragment, "FRAGMENT");

		unsigned int geometry = 0;
		if (geometry_path != nullptr)
		{
			auto g_shader_code = geometry_code.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &g_shader_code, nullptr);
			glCompileShader(geometry);
			check_compile_errors(geometry, "GEOMETRY");
		}

		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		if (geometry_path != nullptr)
			glAttachShader(id, geometry);
		glLinkProgram(id);
		check_compile_errors(id, "PROGRAM");

		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometry_path != nullptr)
			glDeleteShader(geometry);
	}

	void use() const
	{
		glUseProgram(id);
	}

	void set_bool(const std::string& name, const bool value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void set_int(const std::string& name, const int value) const
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void set_float(const std::string& name, const float value) const
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void set_vec2(const std::string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void set_vec2(const std::string& name, const float x, const float y) const
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}

	void set_vec3(const std::string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void set_vec3(const std::string& name, const float x, const float y, const float z) const
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}

	void set_vec4(const std::string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}

	void set_vec4(const std::string& name, const float x, const float y, const float z, const float w) const
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}

	void set_mat2(const std::string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void set_mat3(const std::string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void set_mat4(const std::string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

private:
	static void check_compile_errors(const GLuint shader, const std::string type)
	{
		GLint success;
		GLchar info_log[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, nullptr, info_log);
				std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << info_log <<
					"\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, nullptr, info_log);
				std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << info_log <<
					"\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}
};

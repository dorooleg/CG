#pragma once
#include <glad/glad.h>
#include "view.h"
#include <memory>
#include <shader_m.h>
#include <glm/gtc/matrix_transform.hpp>
#include <camera.h>

class cube
{
public:
	cube()
	{
		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, get_vertices_bytes(), get_vertices(), GL_STATIC_DRAW);
	}

	~cube()
	{
		glDeleteBuffers(1, &vbo_);
	}

	static const float* get_vertices() { return vertices; }

	static size_t get_vertices_size() { return sizeof vertices / sizeof(float) / 6; }

	static size_t get_vertices_bytes() { return sizeof vertices; }

	std::unique_ptr<view> get_view(const Shader& shader,
									const unsigned int scr_width,
									const unsigned int scr_height,
									glm::vec3& light_pos,
									Camera &camera) const
	{
		return std::make_unique<cube_view>(vbo_, shader, scr_width, scr_height, get_vertices_size(), light_pos, camera);
	}

private:
	class cube_view : public view
	{
	public:
		explicit cube_view(const unsigned int vbo,
							const Shader& shader,
							const unsigned int scr_width,
							const unsigned int scr_height,
							const size_t count_vertex,
							glm::vec3& light_pos,
							Camera &camera)
			: scr_width_(scr_width)
			, scr_height_(scr_height)
			, count_vertex_(count_vertex)
			, shader_(shader)
			, light_pos_(light_pos)
			, camera_(camera)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glGenVertexArrays(1, &vao_);
			glBindVertexArray(vao_);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void*>(nullptr));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		}

		void set_shader(const Shader* shader) override
		{
			//shader_ = shader;
		}

		void draw() override
		{
			// also draw the lamp object
			shader_.use();
			const auto projection = glm::perspective(glm::radians(camera_.Zoom), float(scr_width_) / float(scr_height_), 0.1f, 100.0f);
			const auto view = camera_.GetViewMatrix();

			shader_.setMat4("projection", projection);
			shader_.setMat4("view", view);
			auto model = glm::mat4();
			model = translate(model, light_pos_);
			model = scale(model, glm::vec3(0.2f)); // a smaller cube
			shader_.setMat4("model", model);

			glBindVertexArray(vao_);
			glDrawArrays(GL_TRIANGLES, 0, count_vertex_);
		}

		virtual ~cube_view()
		{
			glDeleteVertexArrays(1, &vao_);
		}

	private:
		const unsigned int scr_width_;
		const unsigned int scr_height_;
		size_t count_vertex_;
		const Shader &shader_;
		glm::vec3& light_pos_;
		Camera& camera_;
		unsigned int vao_;

	};

private:
	unsigned int vbo_;
	static constexpr float vertices[] {
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		}; 
};

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
	cube();

	~cube();

	static const float* get_vertices();

	static size_t get_vertices_size();

	static size_t get_vertices_bytes();

	std::unique_ptr<view> get_view(const Shader& shader,
	                               const unsigned int scr_width,
	                               const unsigned int scr_height,
	                               glm::vec3& light_pos,
	                               Camera& camera) const;

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
		                   Camera& camera);

		void set_shader(const Shader* shader) override;

		void draw() override;

		virtual ~cube_view();

	private:
		const unsigned int scr_width_;
		const unsigned int scr_height_;
		size_t count_vertex_;
		const Shader &shader_;
		glm::vec3& light_pos_;
		Camera& camera_;
		unsigned int vao_;

	};

	unsigned int vbo_;
	static constexpr const float vertices[] {
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

inline cube::cube()
{
	glGenBuffers(1, &vbo_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glBufferData(GL_ARRAY_BUFFER, get_vertices_bytes(), get_vertices(), GL_STATIC_DRAW);
}

inline cube::~cube()
{
	glDeleteBuffers(1, &vbo_);
}

constexpr const float cube::vertices[];
inline const float* cube::get_vertices()
{
	return vertices;
}

inline size_t cube::get_vertices_size()
{
	return sizeof vertices / sizeof(float) / 6;
}

inline size_t cube::get_vertices_bytes()
{
	return sizeof vertices;
}

inline std::unique_ptr<view> cube::get_view(const Shader& shader, const unsigned scr_width, const unsigned scr_height,
                                            glm::vec3& light_pos, Camera& camera) const
{
	return std::make_unique<cube_view>(vbo_, shader, scr_width, scr_height, get_vertices_size(), light_pos, camera);
}

inline cube::cube_view::cube_view(const unsigned vbo, const Shader& shader, const unsigned scr_width,
                                  const unsigned scr_height, const size_t count_vertex, glm::vec3& light_pos,
                                  Camera& camera): scr_width_(scr_width)
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

inline void cube::cube_view::set_shader(const Shader* shader)
{
	//shader_ = shader;
}

inline void cube::cube_view::draw()
{
	// also draw the lamp object
	shader_.use();
	const auto projection = glm::perspective(glm::radians(camera_.Zoom), float(scr_width_) / float(scr_height_), 0.1f,
	                                         100.0f);
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

inline cube::cube_view::~cube_view()
{
	glDeleteVertexArrays(1, &vao_);
}

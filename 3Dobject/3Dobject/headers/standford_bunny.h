#pragma once

#include <wavefront.h>
#include <view.h>
#include <memory>
#include <shader_m.h>
#include <camera.h>

class stanford_bunny
{
public:
	stanford_bunny()
	{
		wavefront object(path);
		const auto& attribute = object.get_attributes();
		object.get_attributes();
		object.get_shapes();

		if (object.get_shapes().empty())
		{
			return;
		}

		for (const auto& idx : object.get_shapes()[0].mesh.indices)
		{
			for (size_t repeat = 0; repeat < 2; repeat++)
			{
				vertices_.insert(vertices_.end(),
					attribute.vertices.begin() + 3 * idx.vertex_index,
					attribute.vertices.begin() + 3 * idx.vertex_index + 3);
			}
		}

		glGenBuffers(1, &vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, get_vertices_bytes(), get_vertices(), GL_STATIC_DRAW);
	}

	const float* get_vertices() const { return &vertices_.front();  }

	size_t get_vertices_size() const { return vertices_.size();  }

	size_t get_vertices_bytes() const { return vertices_.size() * sizeof(float); }

	std::unique_ptr<view> get_view(const Shader& shader,							 
									const unsigned int scr_width,
									const unsigned int scr_height,
									glm::vec3& light_pos,
									glm::vec3& pos,
									const glm::vec3 ambient,
									const glm::vec3 diffuse,
									const glm::vec3 specular,
									const float shininess,
									Camera &camera) const
	{
		return std::make_unique<standford_bunny>(vbo_, shader, scr_width, scr_height, get_vertices_size(), light_pos, pos, ambient, diffuse, specular, shininess, camera);
	}

	~stanford_bunny()
	{
		glDeleteBuffers(1, &vbo_);
	}

private:
	class standford_bunny : public view
	{
	public:
		explicit standford_bunny(const unsigned int vbo,
								  const Shader& shader,
								  const unsigned int scr_width,
								  const unsigned int scr_height,
		                          const size_t count_vertex,
		                          glm::vec3& light_pos,
								  glm::vec3& pos,
								  const glm::vec3 ambient,
								  const glm::vec3 diffuse,
								  const glm::vec3 specular,
								  const float shininess,
		                          Camera &camera)
			: scr_width_(scr_width)
			, scr_height_(scr_height)
			, count_vertex_(count_vertex)
			, light_pos_(light_pos)
			, pos_(pos)
			, ambient_(ambient)
			, diffuse_(diffuse)
			, specular_(specular)
			, shininess_(shininess)
			, camera_(camera)
			, shader_(shader)
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
			shadow_shader_ = shader;
		}

		void draw() override
		{
			// be sure to activate shader when setting uniforms/drawing objects
			if (shadow_shader_ == nullptr)
			{
				shader_.use();

				glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

				glm::mat4 lightProjection, lightView;
				glm::mat4 lightSpaceMatrix;
				float near_plane = 1.0f, far_plane = 7.5f;
				//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene
				lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
				lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMatrix = lightProjection * lightView;

				shader_.setVec3("lightPos", lightPos);
				shader_.setMat4("lightSpaceMatrix", lightSpaceMatrix);


				shader_.setVec3("light.position", light_pos_);
				shader_.setVec3("viewPos", camera_.Position);

				// light properties
				shader_.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
				shader_.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
				shader_.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

				// material properties
				//shader_.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
				//shader_.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
				//shader_.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
				//shader_.setFloat("material.shininess", 32.0f);
				shader_.setVec3("material.ambient", ambient_);
				shader_.setVec3("material.diffuse", diffuse_);
				shader_.setVec3("material.specular", specular_); // specular lighting doesn't have full effect on this object's material
				shader_.setFloat("material.shininess", shininess_);

				// view/projection transformations
				const auto projection = glm::perspective(glm::radians(camera_.Zoom), float(scr_width_) / float(scr_height_), 0.1f, 100.0f);
				const auto view = camera_.GetViewMatrix();
				shader_.setMat4("projection", projection);
				shader_.setMat4("view", view);
			}

			// world transformation
			glm::mat4 model;
			model = translate(model, pos_);
			model = scale(model, glm::vec3(5.0f));
			
			if (shadow_shader_ == nullptr)
				shader_.setMat4("model", model);
			else
				shadow_shader_->setMat4("model", model);

			// render the cube
			glBindVertexArray(vao_);
			glDrawArrays(GL_TRIANGLES, 0, count_vertex_);
		}

		virtual ~standford_bunny()
		{
			glDeleteVertexArrays(1, &vao_);
		}

	private:
		const unsigned int scr_width_;
		const unsigned int scr_height_;
		size_t count_vertex_;
		glm::vec3& light_pos_;
		glm::vec3& pos_;
		glm::vec3 ambient_;
		glm::vec3 diffuse_;
		glm::vec3 specular_;
		float shininess_;
		Camera& camera_;
		const Shader &shader_;
		const Shader *shadow_shader_ = nullptr;
		unsigned int vao_;
	};

private:
	static constexpr char *path = "../3DObject/resources/objs/stanford_bunny.obj";
	std::vector<float> vertices_;
	unsigned int vbo_;
};

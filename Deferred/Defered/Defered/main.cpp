#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/shader.h>
#include <common/camera.h>
#include <common/model.h>

#include <iostream>
#include <ctime>
#include <string>

const string shaders_path = "../Defered/shaders/";
const string objects_path = "../Defered/objects/";

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
static void process_input(GLFWwindow* window);
static void render_quad();
static void render_cube();
static glm::vec3 get_random_position();
static glm::vec3 get_random_color();
static void resize_count_lights(std::vector<glm::vec3> &positions, vector<glm::vec3> &colors, const size_t size);

const unsigned int scr_width = 1280;
const unsigned int scr_height = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
float last_x = static_cast<float>(scr_width) / 2.0;
float last_y = static_cast<float>(scr_height) / 2.0;
bool first_mouse = true;

float delta_time = 0.0f;
float last_frame = 0.0f;
float gamma_correction = 2.2f;

enum TypeRender
{
	ALL,
	POSITION,
	NORMAL,
	DIFFUSE,
	SPECULAR
};

TypeRender type_render = ALL;
int number_lights = 20;

vector<glm::vec3> light_positions;
vector<glm::vec3> light_colors;

int main()
{
	srand(time(nullptr));

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	const auto window = glfwCreateWindow(scr_width, scr_height, "Deferred", nullptr, nullptr);
	if (window == nullptr)
	{
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		cerr << "Failed to initialize GLAD" << endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader shader_geometry_pass((shaders_path + "g_buffer.vs").c_str(), (shaders_path + "g_buffer.fs").c_str());
	Shader shader_lighting_pass((shaders_path + "deferred_shading.vs").c_str(),
	                          (shaders_path + "deferred_shading.fs").c_str());
	Shader shader_light_box((shaders_path + "deferred_light_box.vs").c_str(),
	                      (shaders_path + "deferred_light_box.fs").c_str());

	Model stanford_bunny(objects_path + "stanford_bunny.obj");
	vector<glm::vec3> object_positions;
	object_positions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	object_positions.push_back(glm::vec3(0.0, -3.0, -3.0));
	object_positions.push_back(glm::vec3(3.0, -3.0, -3.0));
	object_positions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	object_positions.push_back(glm::vec3(0.0, -3.0, 0.0));
	object_positions.push_back(glm::vec3(3.0, -3.0, 0.0));
	object_positions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	object_positions.push_back(glm::vec3(0.0, -3.0, 3.0));
	object_positions.push_back(glm::vec3(3.0, -3.0, 3.0));

	unsigned int g_buffer;
	glGenFramebuffers(1, &g_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	unsigned int g_position, g_normal, g_albedo_spec;

	glGenTextures(1, &g_position);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, scr_width, scr_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);
	// normal color buffer
	glGenTextures(1, &g_normal);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, scr_width, scr_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);
	// color + specular color buffer
	glGenTextures(1, &g_albedo_spec);
	glBindTexture(GL_TEXTURE_2D, g_albedo_spec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scr_width, scr_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_albedo_spec, 0);

	unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

	unsigned int rbo_depth;
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "Framebuffer not complete!" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	resize_count_lights(light_positions, light_colors, number_lights);

	shader_lighting_pass.use();
	shader_lighting_pass.set_int("gPosition", 0);
	shader_lighting_pass.set_int("gNormal", 1);
	shader_lighting_pass.set_int("gAlbedoSpec", 2);
	
	while (!glfwWindowShouldClose(window))
	{
		const float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		process_input(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		const auto projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(scr_width) / static_cast<float>(scr_height), 0.1f, 100.0f);
		const auto view = camera.get_view_matrix();
		glm::mat4 model;
		shader_geometry_pass.use();
		shader_geometry_pass.set_mat4("projection", projection);
		shader_geometry_pass.set_mat4("view", view);
		for (unsigned int i = 0; i < object_positions.size(); i++)
		{
			model = glm::mat4();
			model = translate(model, object_positions[i]);
			model = scale(model, glm::vec3(0.25f));
			shader_geometry_pass.set_mat4("model", model);
			stanford_bunny.draw();
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader_lighting_pass.use();
		shader_lighting_pass.set_int("lightCount", number_lights);
		shader_lighting_pass.set_int("typeRender", type_render);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_position);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_albedo_spec);
		for (unsigned int i = 0; i < light_positions.size(); i++)
		{
			static const float linear = 0.7;
			static const float quadratic = 1.8;

			shader_lighting_pass.set_vec3("lights[" + to_string(i) + "].Position", light_positions[i]);
			shader_lighting_pass.set_vec3("lights[" + to_string(i) + "].Color", light_colors[i]);

			shader_lighting_pass.set_float("lights[" + to_string(i) + "].Linear", linear);
			shader_lighting_pass.set_float("lights[" + to_string(i) + "].Quadratic", quadratic);
		}
		shader_lighting_pass.set_vec3("viewPos", camera.Position);
		shader_lighting_pass.set_float("gammaCorrection", gamma_correction);
		render_quad();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, g_buffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, scr_width, scr_height, 0, 0, scr_width, scr_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		shader_light_box.use();
		shader_light_box.set_mat4("projection", projection);
		shader_light_box.set_mat4("view", view);
		for (unsigned int i = 0; i < light_positions.size(); i++)
		{
			model = glm::mat4();
			model = translate(model, light_positions[i]);
			model = scale(model, glm::vec3(0.125f));
			shader_light_box.set_mat4("model", model);
			shader_light_box.set_vec3("lightColor", light_colors[i]);
			render_cube();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

static void render_cube()
{
	static unsigned int cube_vao = 0;
	static unsigned int cube_vbo = 0;
	if (cube_vao == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			1.0f, 1.0f , 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right     
			1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
			-1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left        
		};
		glGenVertexArrays(1, &cube_vao);
		glGenBuffers(1, &cube_vbo);

		glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindVertexArray(cube_vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)nullptr);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

static void render_quad()
{
	static unsigned int quad_vao = 0;
	static unsigned int quad_vbo = 0;

	if (quad_vao == 0)
	{
		float quad_vertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quad_vao);
		glGenBuffers(1, &quad_vbo);
		glBindVertexArray(quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof quad_vertices, &quad_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void*>(nullptr));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	}
	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

static void process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.process_keyboard(FORWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.process_keyboard(BACKWARD, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.process_keyboard(LEFT, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.process_keyboard(RIGHT, delta_time);
	}

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		type_render = ALL;
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		type_render = POSITION;
	}

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		type_render = NORMAL;
	}

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		type_render = DIFFUSE;
	}

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		type_render = SPECULAR;
	}

	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		number_lights++;
		resize_count_lights(light_positions, light_colors, number_lights);
	}

	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		number_lights = max(number_lights - 1, 0);
		resize_count_lights(light_positions, light_colors, number_lights);
	}

	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		gamma_correction += 0.1;
	}

	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		gamma_correction = max(0.1, gamma_correction - 0.1);
	}
}

static void framebuffer_size_callback(GLFWwindow* /*window*/, const int width, const int height)
{
	glViewport(0, 0, width, height);
}

static void mouse_callback(GLFWwindow* /*window*/, const double xpos, const double ypos)
{
	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	const float xoffset = xpos - last_x;
	const float yoffset = last_y - ypos;

	last_x = xpos;
	last_y = ypos;

	camera.process_mouse_movement(xoffset, yoffset);
}

static void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, const double yoffset)
{
	camera.process_mouse_scroll(yoffset);
}

static glm::vec3 get_random_position()
{
	const float x = rand() % 100 / 100.0 * 6.0 - 3.0;
	const float y = rand() % 100 / 100.0 * 6.0 - 4.0;
	const float z = rand() % 100 / 100.0 * 6.0 - 3.0;
	return glm::vec3(x, y, z);
}

static glm::vec3 get_random_color()
{
	const float r = rand() % 100 / 200.0f + 0.5;
	const float g = rand() % 100 / 200.0f + 0.5;
	const float b = rand() % 100 / 200.0f + 0.5;
	return glm::vec3(r, g, b);
}

static void resize_count_lights(std::vector<glm::vec3> &positions, vector<glm::vec3> &colors, const size_t size)
{
	const auto min_size = min(positions.size(), colors.size());

	for (auto i = min_size; i <= size; i++)
	{
		positions.push_back(get_random_position());
		colors.push_back(get_random_color());
	}

	positions.resize(size);
	colors.resize(size);
}

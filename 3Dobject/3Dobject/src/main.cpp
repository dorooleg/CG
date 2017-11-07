#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>

#include <standford_bunny.h>

#include <iostream>
#include <cube.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);

// settings
const unsigned int scr_width = 800;
const unsigned int scr_height = 600;

// camera
Camera camera(glm::vec3(1.0f, 2.f, 10.0f));
float last_x = scr_width / 2.0f;
float last_y = scr_height / 2.0f;
bool first_mouse = true;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

// lighting
glm::vec3 light_pos(1.2f, 1.0f, 2.0f);

// meshes
unsigned int plane_vao;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	const auto window = glfwCreateWindow(scr_width, scr_height, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader zprogram
	// ------------------------------------
	const Shader lamp_shader("../3Dobject/shaders/lamp.vs", "../3Dobject/shaders/lamp.fs");
	const Shader shader("../3Dobject/shaders/shadow_mapping.vs", "../3Dobject/shaders/shadow_mapping.fs");
	const Shader simple_depth_shader("../3Dobject/shaders/shadow_mapping_depth.vs", "../3Dobject/shaders/shadow_mapping_depth.fs");

	float plane_vertices[] = {
		// positions            // normals         // texcoords
		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

		25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
		-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
		25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
	};

	// plane VAO
	unsigned int plane_vbo;
	glGenVertexArrays(1, &plane_vao);
	glGenBuffers(1, &plane_vbo);
	glBindVertexArray(plane_vao);
	glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), plane_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), static_cast<void*>(nullptr));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void*>(6 * sizeof(float)));
	glBindVertexArray(0);

	stanford_bunny bunny;
	cube cube;

	glm::vec3 static_pos_1(0, 0, 0);
	auto bunny_static_1 = bunny.get_view(shader, 
								scr_width,
								scr_height, 
								light_pos, 
								static_pos_1, 
								glm::vec3(1, 0.7, 0.31),
								glm::vec3(1, 0.7, 0.31),
								glm::vec3(0.7, 0.7, 0.5),
								32.0,
								camera);

	glm::vec3 static_pos_2(1, 1, 1);
	auto bunny_static_2 = bunny.get_view(shader,
										scr_width,
										scr_height,
										light_pos,
										static_pos_2,
										glm::vec3(0.1, 0.7, 0.31),
										glm::vec3(0.8, 0.7, 0.36),
										glm::vec3(0.7, 0.2, 0.2),
										64.0,
										camera);

	glm::vec3 dynamic_pos_1(-1, -1, -1);
	auto bunny_dynamic_1 = bunny.get_view(shader,
										scr_width,
										scr_height,
										light_pos,
										dynamic_pos_1,
										glm::vec3(0.5, 0.5, 0.5),
										glm::vec3(0.5, 0.5, 0.5),
										glm::vec3(0.5, 0.5, 0.5),
										1.0,
										camera);

	glm::vec3 dynamic_pos_2(0, 1, 1);
	auto bunny_dynamic_2 = bunny.get_view(shader,
										scr_width,
										scr_height,
										light_pos,
										dynamic_pos_2,
										glm::vec3(0.1, 0.5, 0.9),
										glm::vec3(0.8, 0.5, 0.9),
										glm::vec3(0.7, 0.2, 0.9),
										64.0,
										camera);

	auto point_light_dynamic = cube.get_view(lamp_shader, scr_width, scr_height, light_pos, camera);

	// configure depth map FBO
	// -----------------------
	const unsigned int shadow_width = 1024, shadow_height = 1024;
	unsigned int depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);
	// create depth texture
	unsigned int depth_map;
	glGenTextures(1, &depth_map);
	glBindTexture(GL_TEXTURE_2D, depth_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float border_color[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// shader configuration
	// --------------------
	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("shadowMap", 1);

	// lighting info
	// -------------
	const glm::vec3 global_light_pos(-2.0f, 4.0f, -1.0f);

	while (!glfwWindowShouldClose(window))
	{
		const float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		process_input(window);

		light_pos.x = 1.2f + 5.0f * sin(current_frame) * cos(delta_time);
		light_pos.y = 1.0f + 5.0f * sin(current_frame) * sin(delta_time);
		light_pos.z = 2.0f + 5.0f * cos(current_frame);

		dynamic_pos_1.x = 1.f + 1.0f * sin(current_frame) * cos(delta_time);
		dynamic_pos_1.y = 1.f + 1.0f * sin(current_frame) * sin(delta_time);
		dynamic_pos_1.z = 1.f + 1.0f * cos(current_frame);

		dynamic_pos_2.x = 1.f - 2.0f * sin(current_frame) * cos(current_frame);
		dynamic_pos_2.y = 2.0f - 2.0f * sin(current_frame) * sin(current_frame);
		dynamic_pos_2.z = 2.0f - 2.0f * cos(current_frame);

		const auto near_plane = 1.0f;
		const auto far_plane = 7.5f;
		const auto light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		const auto light_view = lookAt(global_light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		const auto light_space_matrix = light_projection * light_view;

		simple_depth_shader.use();
		simple_depth_shader.setMat4("lightSpaceMatrix", light_space_matrix);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, shadow_width, shadow_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);

			bunny_static_1->set_shader(&simple_depth_shader);
			bunny_static_2->set_shader(&simple_depth_shader);
			bunny_dynamic_1->set_shader(&simple_depth_shader);
			bunny_dynamic_2->set_shader(&simple_depth_shader);

			bunny_static_1->draw();
			bunny_static_2->draw();
			bunny_dynamic_1->draw();
			bunny_dynamic_2->draw();

			glm::mat4 model;
			simple_depth_shader.setMat4("model", model);
			glBindVertexArray(plane_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisable(GL_POLYGON_OFFSET_FILL);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		bunny_static_1->set_shader(nullptr);
		bunny_static_2->set_shader(nullptr);
		bunny_dynamic_1->set_shader(nullptr);
		bunny_dynamic_2->set_shader(nullptr);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f);

		bunny_static_1->draw();
		bunny_static_2->draw();
		bunny_dynamic_1->draw();
		bunny_dynamic_2->draw();

		glDisable(GL_POLYGON_OFFSET_FILL);
		
		shader.use();
		const auto projection = glm::perspective(glm::radians(camera.Zoom), float(scr_width) / float(scr_height), 0.1f, 100.0f);
		const auto view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", global_light_pos);
		shader.setMat4("lightSpaceMatrix", light_space_matrix);
		shader.setVec3("light.position", light_pos);
		shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
		// material properties
		shader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		shader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		shader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		shader.setFloat("material.shininess", 32.0f);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depth_map);

		model = glm::mat4();
		shader.setMat4("model", model);
		glBindVertexArray(plane_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		point_light_dynamic->draw();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void process_input(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, delta_time);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, delta_time);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, delta_time);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* /*window*/, const int width, const int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* /*window*/, const double xpos, const double ypos)
{
	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}

	const float xoffset = xpos - last_x;
	const float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

	last_x = xpos;
	last_y = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, const double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
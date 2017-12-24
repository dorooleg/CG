#pragma once

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float yaw        = -90.0f;
const float pitch      =  0.0f;
const float speed      =  2.5f;
const float sensitivty =  0.1f;
const float zoom       =  45.0f;

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

	explicit Camera(const glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = yaw, float pitch = pitch)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f))
		, MovementSpeed(speed)
		, MouseSensitivity(sensitivty)
		, Zoom(zoom)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        update_camera_vectors();
    }

    Camera(const float pos_x, const float pos_y, const float pos_z, const float up_x, const float up_y, const float up_z, const float yaw, const float pitch)
		: Front(glm::vec3(0.0f, 0.0f, -1.0f))
		, MovementSpeed(speed)
		, MouseSensitivity(sensitivty)
		, Zoom(zoom)
    {
        Position = glm::vec3(pos_x, pos_y, pos_z);
        WorldUp = glm::vec3(up_x, up_y, up_z);
        Yaw = yaw;
        Pitch = pitch;
        update_camera_vectors();
    }

    glm::mat4 get_view_matrix() const
    {
        return lookAt(Position, Position + Front, Up);
    }

	void process_keyboard(const Camera_Movement direction, const float delta_time)
    {
	    const auto velocity = MovementSpeed * delta_time;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

	void process_mouse_movement(float xoffset, float yoffset, const GLboolean constrain_pitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        if (constrain_pitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        update_camera_vectors();
    }

	void process_mouse_scroll(const float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:
    void update_camera_vectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = normalize(front);
        Right = normalize(cross(Front, WorldUp));
        Up = normalize(cross(Right, Front));
    }
};
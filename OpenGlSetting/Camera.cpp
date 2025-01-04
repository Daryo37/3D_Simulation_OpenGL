#include "Camera.h"


Camera::Camera()
{
	front = glm::vec3(0.0f, 0.0f, 0.0f);
	moveSpeed = 0.0f;
	turnSpeed = 0.0f;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	worldUp = glm::vec3(0.0f, 0.0f, 0.0f);
	yaw = 0.0f;
	pitch = 0.0f;
	right = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, 0.0f);
}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw,GLfloat startPitch,GLfloat startMovespeed,GLfloat startTurnspeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;

	front = glm::vec3( 0.0f, 0.0f, -1.0f );

	moveSpeed = startMovespeed;
	turnSpeed = startTurnspeed;

	update();

}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin( glm::radians( yaw ) );

	front = glm::normalize(front);

	right = glm::normalize(cross(front, worldUp));
	up = glm::normalize(cross(right,front));

}

void Camera::keyControl(bool* keys,GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

	if (keys[GLFW_KEY_W])
	{
		position += front * velocity;
	}
	if (keys[GLFW_KEY_S])
	{
		position -= front * velocity;
	}
	if (keys[GLFW_KEY_A])
	{
		position -= right * velocity;
	}
	if (keys[GLFW_KEY_D])
	{
		position += right * velocity;
	}
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}
	update();
}
glm::vec3 Camera::getCameraPosition()
{
	return position;
}

glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front,up);
}




Camera::~Camera()
{

}

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Input/Keyboard.h"

enum class CameraDirection {
	NONE,
	FORWARD,
	BACKWARD,
	LEFTWARD,
	RIGHTWARD,
	UPWARD,
	DOWNWARD
};

class Camera {
public:
	Camera() = default;
	Camera(glm::vec3 position);

	glm::vec3 cameraPos;

	glm::vec3 cameraUp;
	glm::vec3 cameraFront;
	glm::vec3 cameraRight;

	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float speed;
	float zoom;

	void updateCameraDirection(double dx, double dy);
	void updateCameraPos(CameraDirection dir, double dt);
	void updateCameraZoom(double dy);

	void setPosition(const glm::vec3& position);

	float getZoom();

	glm::mat4 getViewMatrix();

	static Camera defaultCamera;

private:
	void updateCameraVectors();
};
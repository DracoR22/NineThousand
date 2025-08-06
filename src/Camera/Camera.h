#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./Frustum.h"
#include "../Input/Keyboard.h"
#include "../Core/Window.h"

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

	void Update();

	void updateCameraDirection(double dx, double dy);
	void updateCameraPos(CameraDirection dir, double dt);
	void updateCameraZoom(double dy);

	void setPosition(const glm::vec3& position);
	void SetCameraZoom(float targetZoom);

	float getZoom();
	float GetNearPlane() const { return m_nearPlane; };
	float GetFarPlane() const { return m_farPlane; };

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
	Frustum& GetFrustum() { return m_frustum; };

	static Camera defaultCamera;

private:
	void updateCameraVectors();

private:
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	Frustum m_frustum;
	float m_nearPlane = 0.005f;
	float m_farPlane = 256.0f;
	float m_targetZoom = 45.0f;
};
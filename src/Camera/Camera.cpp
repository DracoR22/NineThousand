#include "Camera.h"

Camera Camera::defaultCamera(glm::vec3(1.0f));

Camera::Camera(glm::vec3 position)
    : cameraPos(position),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
    yaw(0.0f),
    pitch(0.0f),
    speed(8.5f),
    zoom(45.0f),
    m_targetZoom(45.0f),
    cameraFront(glm::vec3(1.0f, 0.0f, 0.0f)) {
	updateCameraVectors();
}

void Camera::Update() {
    m_viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    m_projectionMatrix = glm::perspective(glm::radians(zoom), (float)Window::m_windowWidth / (float)Window::m_windowHeight, m_nearPlane, m_farPlane);
    m_frustum.Update(m_projectionMatrix * m_viewMatrix);

    float zoomSpeed = 10.0f;
    float zoomLerpFactor = zoomSpeed * Window::GetDeltaTime();
    zoom = glm::mix(zoom, m_targetZoom, zoomLerpFactor);
}

// change camera direction (mouse movement)
void Camera::updateCameraDirection(double dx, double dy) {
    yaw += dx;
    pitch += dy;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }

    updateCameraVectors();
}

// change camera position (keyboard)
void Camera::updateCameraPos(CameraDirection direction, double dt) {
    float velocity = (float)dt * speed;

    if (Keyboard::KeyPressed(GLFW_KEY_LEFT_CONTROL)) {
        velocity *= 2.0f;  // Double the velocity when running
    }

    switch (direction) {
    case CameraDirection::FORWARD:
        cameraPos += cameraFront * velocity;
        break;
    case CameraDirection::BACKWARD:
        cameraPos -= cameraFront * velocity;
        break;
    case CameraDirection::RIGHTWARD:
        cameraPos += cameraRight * velocity;
        break;
    case CameraDirection::LEFTWARD:
        cameraPos -= cameraRight * velocity;
        break;
    case CameraDirection::UPWARD:
        cameraPos += worldUp * velocity;
        break;
    case CameraDirection::DOWNWARD:
        cameraPos -= worldUp * velocity;
        break;
    }
}

glm::mat4 Camera::GetViewMatrix() {
    return m_viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix() {
    return m_projectionMatrix;
}

float Camera::getZoom() {
    return zoom;
}

void Camera::updateCameraZoom(double dy) {
    if (zoom >= 1.0f && zoom <= 45.0f) {
        zoom -= dy;
    }
    else if (zoom < 1.0f) {
        zoom = 1.0f;
    }
    else { // > 45.0f
        zoom = 45.0f;
    }
}

void Camera::updateCameraVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);

    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
}

void Camera::setPosition(const glm::vec3& position) {
    cameraPos = position;
}

void Camera::SetCameraZoom(float targetZoom) {
    m_targetZoom = glm::clamp(targetZoom, 1.0f, 45.0f);
}
#include "Camera.h"

Camera::Camera(glm::vec3 position)
: cameraPos(position),
worldUp(glm::vec3(0.0f, 1.0f, 0.0f)),
yaw(0.0f),
pitch(0.0f),
speed(2.5f),
zoom(45.0f),
cameraFront(glm::vec3(1.0f, 0.0f, 0.0f)) {
	updateCameraVectors();
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

    if (Keyboard::key(GLFW_KEY_LEFT_CONTROL)) {
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

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

// get zoom value for camera
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
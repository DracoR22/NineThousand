#include "./Editor.h"

namespace Editor {
	Camera g_camera;

	void Init() {
		g_camera = Camera(glm::vec3(5.0f));
	}

	void Update() {
		UpdateCamera();
	}

	void UpdateCamera() {
		if (Keyboard::KeyPressed(GLFW_KEY_W)) {
			g_camera.updateCameraPos(CameraDirection::FORWARD, Window::GetDeltaTime());
		}
		if (Keyboard::KeyPressed(GLFW_KEY_S)) {
			g_camera.updateCameraPos(CameraDirection::BACKWARD, Window::GetDeltaTime());
		}
		if (Keyboard::KeyPressed(GLFW_KEY_D)) {
			g_camera.updateCameraPos(CameraDirection::RIGHTWARD, Window::GetDeltaTime());
		}
		if (Keyboard::KeyPressed(GLFW_KEY_A)) {
			g_camera.updateCameraPos(CameraDirection::LEFTWARD, Window::GetDeltaTime());
		}
		if (Keyboard::KeyPressed(GLFW_KEY_SPACE)) {
			g_camera.updateCameraPos(CameraDirection::UPWARD, Window::GetDeltaTime());
		}
		if (Keyboard::KeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			g_camera.updateCameraPos(CameraDirection::DOWNWARD, Window::GetDeltaTime());
		}

		if (Mouse::button(GLFW_MOUSE_BUTTON_MIDDLE)) {
			static glm::vec2 smoothedDelta = glm::vec2(0.0f);
			const float smoothingFactor = 0.55f;

			double dx = Mouse::getDX();
			double dy = Mouse::getDY();

			// Smooth mouse input
			smoothedDelta.x += (dx - smoothedDelta.x) * smoothingFactor;
			smoothedDelta.y += (dy - smoothedDelta.y) * smoothingFactor;

			float sensitivity = 0.17f;
			g_camera.updateCameraDirection(smoothedDelta.x * sensitivity, smoothedDelta.y * sensitivity);
		}
	}

	Camera& GetCamera() {
		return g_camera;
	}
}
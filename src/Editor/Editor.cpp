#include "./Editor.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Editor {
	Camera g_camera;
	glm::vec3 g_rayOrigin;
	glm::vec3 g_rayDirection;

	void Init() {
		g_camera = Camera(glm::vec3(5.0f));
	}

	void Update() {
		UpdateCamera();
		UpdateMouseRays();

		/*if (Mouse::buttonWentDown(GLFW_MOUSE_BUTTON_LEFT)) {
			GameObject* plane = Scene::GetGameObjectByName("Plane1");
			glm::vec3 localUp(0, 1, 0);
			glm::vec3 planeNormal = glm::normalize(glm::vec3(plane->GetRotationMatrix() * glm::vec4(localUp, 0.0f)));

			glm::vec3 hit = GetMouseRayPlaneIntersection(plane->GetPosition(), planeNormal);

			std::cout << "HIT OBJECT AT " << glm::to_string(hit) << std::endl;
		}*/
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

		if (Mouse::ButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
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

	void UpdateMouseRays() {
		glm::mat4 viewMatrx = CameraManager::GetActiveCamera()->GetViewMatrix();
		glm::mat4 projectionMatrix = CameraManager::GetActiveCamera()->GetProjectionMatrix();
		float x = (2.0f * Mouse::getMouseX()) / Window::currentWidth -1.0f;
		float y = 1.0f - (2.0f * Mouse::getMouseY()) / Window::currentHeight;
		glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

		glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

		g_rayDirection = glm::normalize(glm::vec3(glm::inverse(viewMatrx) * rayEye));
		g_rayOrigin = glm::vec3(glm::inverse(viewMatrx)[3]);
	}

	glm::vec3 GetMouseRayPlaneIntersection(glm::vec3 planeOrigin, glm::vec3 planeNormal) {
		const glm::vec3 rayOrigin = GetMouseRayOrigin();
		const glm::vec3 rayDirection = GetMouseRayDirection();
		float distanceToHit = 0;
		bool hitFound = glm::intersectRayPlane(g_rayOrigin, g_rayDirection, planeOrigin, planeNormal, distanceToHit);

		if (hitFound && distanceToHit > 0.0f) {
			return rayOrigin + distanceToHit * rayDirection;
		}

		return glm::vec3(0.0f);
	}

	glm::vec3 GetMouseRayOrigin() {
		return g_rayOrigin;
	}

	glm::vec3 GetMouseRayDirection() {
		return g_rayDirection;
	}

	Camera& GetCamera() {
		return g_camera;
	}
}
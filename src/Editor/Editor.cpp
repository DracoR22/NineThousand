#include "./Editor.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Editor {
	Camera g_camera;
	glm::vec3 g_rayOrigin;
	glm::vec3 g_rayDirection;

	int g_viewportWidth = Window::m_windowWidth;
	int g_viewportHeight = Window::m_windowHeight;
	int g_viewportPosX = 0;
	int g_viewportPosY = 0;

	void Init() {
		g_camera = Camera(glm::vec3(5.0f));
	}

	void Update() {
		UpdateCamera();
		UpdateMouseRays();

		// update physics collitions
		for (GameObject& gameObject : Scene::GetGameObjects()) {
			if (gameObject.GetPhysicsId() > 0) {
				Physics::SetRigidDynamicGlobalPose(gameObject.GetPhysicsId(), gameObject.GetModelMatrix());
				Physics::SetRigidStaticGlobalPose(gameObject.GetPhysicsId(), gameObject.GetModelMatrix());
			}
		}

		if (Mouse::ButtonJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
			PxRaycastBuffer hit;
			PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC);

			bool status = Physics::GetScene()->raycast(
				PxVec3(g_rayOrigin.x, g_rayOrigin.y, g_rayOrigin.z),    // origin
				PxVec3(g_rayDirection.x, g_rayDirection.y, g_rayDirection.z), // direction (normalized)
				1000.0f,                                                // max distance
				hit,
				PxHitFlag::eDEFAULT,                                    // hit info flags
				filterData                                              // query filter
			);

			glm::vec2 mousePos = glm::vec2(Mouse::getMouseX(), Mouse::getMouseY());

			glm::vec2 viewportPos = GetViewportPos();
			glm::vec2 viewportSize = GetViewportSize();

			// check if mouse is inside viewport rect
			bool mouseInViewport =
				mousePos.x >= viewportPos.x &&
				mousePos.x <= viewportPos.x + viewportSize.x &&
				mousePos.y >= viewportPos.y &&
				mousePos.y <= viewportPos.y + viewportSize.y;

			if (mouseInViewport) {
				if (status && hit.hasBlock) {
					for (GameObject& gameObject : Scene::GetGameObjects()) {
						gameObject.SetSelected(false);

						RigidStatic* rigidStatic = Physics::GetRigidStaticById(gameObject.GetPhysicsId());
						RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(gameObject.GetPhysicsId());

						PxRaycastHit rayHit = hit.block;
						PxRigidActor* actor = rayHit.actor;

						if ((rigidStatic && rigidStatic->GetPxRigidStatic() == actor) || (rigidDynamic && rigidDynamic->GetPxRigidDynamic() == actor)) {
							gameObject.SetSelected(true);
						}
					}
				}
			}
		}
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
		glm::vec2 viewportPos = GetViewportPos();
		glm::vec2 viewportSize = GetViewportSize();

		glm::mat4 viewMatrx = CameraManager::GetActiveCamera()->GetViewMatrix();
		glm::mat4 projectionMatrix = CameraManager::GetActiveCamera()->GetProjectionMatrix();

		float mouseX = (Mouse::getMouseX() - viewportPos.x) / viewportSize.x;
		float mouseY = (Mouse::getMouseY() - viewportPos.y) / viewportSize.y;

		// Convert to NDC
		float x = mouseX * 2.0f - 1.0f;
		float y = 1.0f - mouseY * 2.0f;  // flip Y

	/*	float x = (2.0f * Mouse::getMouseX()) / Window::m_windowWidth -1.0f;
		float y = 1.0f - (2.0f * Mouse::getMouseY()) / Window::m_windowHeight;*/

		glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);

		glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

		glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrx) * rayEye);
		rayWorld = glm::normalize(rayWorld);

		g_rayDirection = rayWorld;
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

	bool RayIntersectsAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t) {
		float t1 = (aabbMin.x - g_rayOrigin.x) / g_rayDirection.x;
		float t2 = (aabbMax.x - g_rayOrigin.x) / g_rayDirection.x;
		float t3 = (aabbMin.y - g_rayOrigin.y) / g_rayDirection.y;
		float t4 = (aabbMax.y - g_rayOrigin.y) / g_rayDirection.y;
		float t5 = (aabbMin.z - g_rayOrigin.z) / g_rayDirection.z;
		float t6 = (aabbMax.z - g_rayOrigin.z) / g_rayDirection.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

		if (tmax < 0) return false;           // AABB is behind ray
		if (tmin > tmax) return false;        // No intersection

		t = tmin;
		return true;

	}

	Camera& GetCamera() {
		return g_camera;
	}

	void SetViewportSize(int width, int height) {
		g_viewportWidth = width;
		g_viewportHeight = height;
	}

	void SetViewportPos(int x, int y) {
		g_viewportPosX = x;
		g_viewportPosY = y;
	}

	glm::vec2 GetViewportPos() {
		return glm::vec2(g_viewportPosX, g_viewportPosY);
	}

	glm::vec2 GetViewportSize() {
		return glm::vec2(g_viewportWidth, g_viewportHeight);
	}
}
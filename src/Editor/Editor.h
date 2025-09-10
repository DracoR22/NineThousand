#pragma once

#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"
#include "../Core/Window.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"
#include "../Game/Scene.h"
#include "../Physics/Physics.h"

namespace Editor {
	void Init();
	void Update();
	void UpdateCamera();
	void UpdateMouseRays();

	glm::vec3 GetMouseRayPlaneIntersection(glm::vec3 planeOrigin, glm::vec3 planeNormal);
	glm::vec3 GetMouseRayOrigin();
	glm::vec3 GetMouseRayDirection();

	/*PhysXRayResult GetPhysXMouseRayHit();*/
	bool RayIntersectsAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax, float& t);

	Camera& GetCamera();

	void SetViewportSize(int width, int height);
	void SetViewportPos(int x, int y);
	glm::vec2 GetViewportPos();
	glm::vec2 GetViewportSize();
}
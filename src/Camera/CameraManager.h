#pragma once

#include "./Camera.h"
#include "../Core/Window.h"
#include <vector>

namespace CameraManager {
	void AddCamera(Camera* camera);
	void SetActiveCamera(int index);
	void Clear();
	Camera* GetActiveCamera();
	void UpdateProjectionMatrix();
	void UpdateViewMatrix();
	glm::mat4 GetProjectionMatrix();
	glm::mat4 GetViewMatrix();
}
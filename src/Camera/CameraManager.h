#pragma once

#include "./Camera.h"
#include "./Frustum.h"
#include "../Core/Window.h"
#include <vector>

namespace CameraManager {
	Camera* GetActiveCamera();
	void AddCamera(Camera* camera);
	void SetActiveCamera(int index);
	void Update();
	void Clear();
}
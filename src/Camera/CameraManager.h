#pragma once

#include "./Camera.h"
#include <vector>

namespace CameraManager {
	void AddCamera(Camera* camera);
	void SetActiveCamera(int index);
	void Clear();
	Camera* GetActiveCamera();
}
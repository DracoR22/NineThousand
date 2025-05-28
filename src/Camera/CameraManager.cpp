#include "./CameraManager.h"

namespace CameraManager {
	std::vector<Camera*> g_cameras;
	int g_activeCameraIndex = 0;

	void AddCamera(Camera* camera) {
		g_cameras.push_back(camera);
	}

	void SetActiveCamera(int index) {
		if (index >= 0 && index < (int)g_cameras.size()) {
			g_activeCameraIndex = index;
		}
	}

	void Clear() {
		g_cameras.clear();
		g_activeCameraIndex = 0;
	}

	Camera* GetActiveCamera() {
		if (g_cameras.empty()) return nullptr;

		return g_cameras[g_activeCameraIndex];
	}
}
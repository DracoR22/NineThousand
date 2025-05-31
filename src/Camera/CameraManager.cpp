#include "./CameraManager.h"

namespace CameraManager {
	std::vector<Camera*> g_cameras;
	int g_activeCameraIndex = 0;
	glm::mat4 g_projectionMatrx = glm::mat4(1.0f);
	glm::mat4 g_viewMatrix = glm::mat4(1.0f);

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

	void UpdateProjectionMatrix() {
		g_projectionMatrx = glm::perspective(glm::radians(GetActiveCamera()->getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 500.0f);
	}

	void UpdateViewMatrix() {
		g_viewMatrix = GetActiveCamera()->getViewMatrix();
	}

	glm::mat4 GetProjectionMatrix() {
		return g_projectionMatrx;
	}

	glm::mat4 GetViewMatrix() {
		return g_viewMatrix;
	}
}
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <iostream>
#include <string>

#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

namespace Window {
 extern	const int SCR_WIDTH;
 extern	const int SCR_HEIGHT;

 extern	int m_windowWidth;
 extern	int m_windowHeight;

 extern	GLFWwindow* window;

	void Init();
	void PrepareFrame();

	void ShowWindow();
	bool WindowShouldClose();
	void SetWindowShouldClose(bool shouldClose);

	void ProcessEvents();
	void ProcessInput(double dt);

	void UpdateDeltaTime();
	float GetDeltaTime();
	void UpdateFPSCount();
	int GetFPSCount();

	void ShutDown();

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
}
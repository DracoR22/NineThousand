#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

namespace Window {
 extern	const int SCR_WIDTH;
 extern	const int SCR_HEIGHT;

 extern	int currentWidth;
 extern	int currentHeight;

 extern	GLFWwindow* window;
	void Init();
	void PrepareFrame();

	bool WindowShouldClose();
	void SetWindowShouldClose(bool shouldClose);

	void ProcessEvents();
	void ProcessInput(double dt);

	void ShutDown();

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
}
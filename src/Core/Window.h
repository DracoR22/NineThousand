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

	void init();
	void processInput(double dt);
	bool windowShouldClose();
	void setWindowShouldClose(bool shouldClose);
	void processEvents(double dt);
	void processInput(double dt);
	void shutDown();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
}
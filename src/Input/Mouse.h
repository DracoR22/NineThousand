#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Mouse {
public:
	static void cursorPosCallback(GLFWwindow* window, double _x, double _y);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseWheelCallback(GLFWwindow* window, double dx, double dy);

	static double getMouseX();
	static double getMouseY();

	static double getDX();
	static double getDY();

	static double getScrollDX();
	static double getScrollDY();

	static bool ButtonPressed(int button);
	static bool ButtonChanged(int button);
	static bool ButtonJustReleased(int button);
	static bool ButtonJustPressed(int button);

	static void ClearDeltas();

private:
	static double x;
	static double y;

	static double lastX;
	static double lastY;

	static double dx;
	static double dy;

	static double scrollDx;
	static double scrollDy;

	static bool firstMouse;

	static bool buttons[];
	static bool buttonsChanged[];
};
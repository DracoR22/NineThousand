#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Keyboard {
public:
	// key state callback
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	// accessors
	static bool KeyPressed(int key);
	static bool KeyJustReleased(int key);
	static bool KeyJustPressed(int key);
	static bool KeyChanged(int key);

private:
	static bool m_keys[];
	static bool m_keysChanged[];
};
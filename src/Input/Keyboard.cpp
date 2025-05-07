#include "Keyboard.h"

bool Keyboard::m_keys[GLFW_KEY_LAST] = { 0 };
bool Keyboard::m_keysChanged[GLFW_KEY_LAST] = { 0 };

void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// ignore media keys for now TODO stuff
	if (key < 0 || key >= 1024) {
		return;
	}

	if (action != GLFW_RELEASE) {
		if (!m_keys[key]) {
			m_keys[key] = true;
		}
	}
	else {
		m_keys[key] = false;
	}

	m_keysChanged[key] = action != GLFW_REPEAT;
}

// accessors
bool Keyboard::KeyPressed(int key) {
	return m_keys[key];
}
bool Keyboard::KeyChanged(int key) {
	bool ret = m_keysChanged[key];
	m_keysChanged[key] = false;

	return ret;
}
bool Keyboard::KeyJustReleased(int key) {
	return !m_keys[key] && KeyChanged(key);
}
bool Keyboard::KeyJustPressed(int key) {
	return m_keys[key] && KeyChanged(key);
}
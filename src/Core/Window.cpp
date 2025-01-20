#include "Window.h"

namespace Window {
	const int SCR_WIDTH = 1200;
	const int SCR_HEIGHT = 800;

	int currentWidth = SCR_WIDTH;
	int currentHeight = SCR_HEIGHT;

	GLFWwindow* window = nullptr;

	void Init() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NineThousand", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return exit(EXIT_FAILURE);
		}

		glfwMakeContextCurrent(window);
		glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

		// inputs
		glfwSetKeyCallback(window, Keyboard::keyCallback);
		glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
		glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
		glfwSetScrollCallback(window, Mouse::mouseWheelCallback);

		// dont show cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return exit(EXIT_FAILURE);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void PrepareFrame() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void ProcessEvents() {
		glfwSwapBuffers(Window::window);
		glfwPollEvents();
	}

	bool WindowShouldClose() {
		return glfwWindowShouldClose(window);
	}

	void SetWindowShouldClose(bool shouldClose) {
		glfwSetWindowShouldClose(Window::window, shouldClose);
	}

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
		currentWidth = width;
		currentHeight = height;

		glViewport(0, 0, width, height);
	}

	void ShutDown() {
		glfwTerminate();
	}

	// TODO: remove this shit, this need to be part of the player class instead
	void ProcessInput(double dt) {
		if (Keyboard::key(GLFW_KEY_ESCAPE)) {
			SetWindowShouldClose(true);
		}

		// move camera
		if (Keyboard::key(GLFW_KEY_W)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_S)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_D)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHTWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_A)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::LEFTWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_SPACE)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::UPWARD, dt);
		}
		if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::DOWNWARD, dt);
		}

		double dx = Mouse::getDX(), dy = Mouse::getDY();
		float sensitivity = 0.1f;
		if (dx != 0 || dy != 0) {
			Camera::defaultCamera.updateCameraDirection(dx * sensitivity, dy * sensitivity);
		}
	}

}
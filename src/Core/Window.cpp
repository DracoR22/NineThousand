#include "Window.h"

namespace Window {
	const int SCR_WIDTH = 1280;
	const int SCR_HEIGHT = 720;

	int currentWidth = SCR_WIDTH;
	int currentHeight = SCR_HEIGHT;

	GLFWwindow* window = nullptr;

	float m_deltaTime = 0.0;
	float m_lastFrame = 0.0f;

	int m_fps = 0;
	const int m_FPS_SMOOTHING_SAMPLES = 50; // Number of frames to average
	double m_fpsBuffer[m_FPS_SMOOTHING_SAMPLES] = { 0 };
	int m_fpsIndex = 0;

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
		/*glfwSwapInterval(1);*/
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

		// enable depth buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		// enble blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// enable gamma correction
		/*glEnable(GL_FRAMEBUFFER_SRGB);*/
	}

	void PrepareFrame() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void ProcessEvents() {
		glfwSwapBuffers(Window::window);
		glfwPollEvents();
	}

	void UpdateDeltaTime() {
		double currentTime = glfwGetTime();
		m_deltaTime = currentTime - m_lastFrame;
		m_lastFrame = currentTime;
	}

	float GetDeltaTime() {
		return m_deltaTime;
	}

	void UpdateFPSCount() {
		if (m_deltaTime > 0.0) {
			double currentFPS = 1.0 / m_deltaTime;

			// Store FPS in the buffer
			m_fpsBuffer[m_fpsIndex] = currentFPS;
			m_fpsIndex = (m_fpsIndex + 1) % m_FPS_SMOOTHING_SAMPLES;

			double fpsSum = 0.0;
			for (int i = 0; i < m_FPS_SMOOTHING_SAMPLES; i++) {
				fpsSum += m_fpsBuffer[i];
			}
			m_fps = fpsSum / m_FPS_SMOOTHING_SAMPLES;
		}
	}

	int GetFPSCount() {
		return m_fps;
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
		if (Keyboard::KeyPressed(GLFW_KEY_ESCAPE)) {
			SetWindowShouldClose(true);
		}

		// move camera
	/*	if (Keyboard::KeyPressed(GLFW_KEY_W)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::FORWARD, dt);
		}
		if (Keyboard::KeyPressed(GLFW_KEY_S)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::BACKWARD, dt);
		}
		if (Keyboard::KeyPressed(GLFW_KEY_D)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::RIGHTWARD, dt);
		}
		if (Keyboard::KeyPressed(GLFW_KEY_A)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::LEFTWARD, dt);
		}
		if (Keyboard::KeyPressed(GLFW_KEY_SPACE)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::UPWARD, dt);
		}
		if (Keyboard::KeyPressed(GLFW_KEY_LEFT_SHIFT)) {
			Camera::defaultCamera.updateCameraPos(CameraDirection::DOWNWARD, dt);
		}

		double dx = Mouse::getDX(), dy = Mouse::getDY();
		float sensitivity = 0.1f;
		if (dx != 0 || dy != 0) {
			Camera::defaultCamera.updateCameraDirection(dx * sensitivity, dy * sensitivity);
		}*/
	}

}
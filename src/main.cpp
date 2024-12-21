#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Graphics/Shader.h"
#include "Graphics/Primitives.hpp"

#include "Input/Camera.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include <stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window, double dt);

int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

Camera Camera::defaultCamera(glm::vec3(0.0f));

int activeCam = 0;

int main() {
	std::cout << "Hello engine" << std::endl;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "NineThousand", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

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
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	// shaders
	Shader shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");

	Model m(glm::vec3(0.0f), glm::vec3(10.0f));
	m.loadModel("resources/models/table/scene.gltf");

	/*Cube cube(glm::vec3(0.0f), glm::vec3(0.75f));
	cube.init();*/

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		processInput(window, deltaTime);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// create transformation for screen
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

		shader.activate();
		shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		/*cube.draw(shader);*/
		m.draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*cube.cleanup();*/
	m.cleanup();

	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window, double dt)
{
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
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
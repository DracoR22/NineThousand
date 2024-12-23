#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Graphics/Shader.h"
#include "Graphics/Primitives.hpp"
#include "Graphics/Cubemap.h"

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
	Shader cubeShader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");
	Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
	Shader mirrorShader("resources/shaders/mirror.vs", "resources/shaders/mirror.fs");

	// skybox
	CubeMap cubemap;

	std::vector<std::string> faces{
		"resources/textures/sky_right.jpg",
		"resources/textures/sky_left.jpg",
		"resources/textures/sky_top.jpg",
		"resources/textures/sky_bottom.jpg",
		"resources/textures/sky_front.jpg",
		"resources/textures/sky_back.jpg",
	};

	cubemap.loadTextures(faces);
	cubemap.init();

	/*Model m(glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(0.05f));
	m.loadModel("resources/models/table/scene.gltf");*/

	Model troll(glm::vec3(0.0f), glm::vec3(0.05f));
	troll.loadModel("resources/models/Glock.fbx");

	Cube cube(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.75f));
	cube.init();

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

	/*	cubeShader.activate();
		cubeShader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		cubeShader.setMat4("view", view);
		cubeShader.setMat4("projection", projection);
		cube.draw(cubeShader);*/

		shader.activate();
		shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		/*mirrorShader.activate();
		mirrorShader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		mirrorShader.setMat4("view", view);
		mirrorShader.setMat4("projection", projection);
		mirrorShader.*/

	/*	m.draw(shader);*/
		troll.draw(shader);
		cube.draw(shader);

		cubemap.render(skyboxShader, Camera::defaultCamera.getViewMatrix(), projection);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cube.cleanup();
	//m.cleanup();
	troll.cleanup();
	cubemap.cleanup();

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
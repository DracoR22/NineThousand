#include "Core/Window.h"

#include <iostream>

#include "Graphics/Shader.h"
#include "Graphics/Primitives.hpp"
#include "Graphics/Cubemap.h"

#include "Input/Camera.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"

#include <stb_image.h>

Camera Camera::defaultCamera(glm::vec3(0.0f));

int activeCam = 0;

int main() {
	std::cout << "Hello engine" << std::endl;
	Window::init();

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

	Cube cube(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.75f));
	cube.init();

	Model troll(glm::vec3(0.0f), glm::vec3(0.05f));
	troll.loadModel("resources/models/Glock.fbx");

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	while (!Window::windowShouldClose()) {
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		/*processInput(Window::window, deltaTime);*/
		Window::processInput(deltaTime);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// create transformation for screen
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		view = Camera::defaultCamera.getViewMatrix();
		projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);

		shader.activate();
		shader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

	/*	m.draw(shader);*/
		cube.draw(shader);

		shader.activate();
		troll.draw(shader);

		cubemap.render(skyboxShader, Camera::defaultCamera.getViewMatrix(), projection);

		/*glfwSwapBuffers(Window::window);
		glfwPollEvents();*/
		Window::processEvents(deltaTime);
	}

	cube.cleanup();
	//m.cleanup();
	troll.cleanup();
	cubemap.cleanup();

	/*glfwTerminate();*/
	Window::shutDown();
	return 0;
}
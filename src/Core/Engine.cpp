#include "Engine.h"

namespace Engine {
	void Run() {
		Window::Init();

		// shaders
		Shader texturedObjectShader("resources/shaders/textured_obj.vs", "resources/shaders/textured_obj.fs");
		Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
		Shader floorPlaneShader("resources/shaders/floor_plane.vs", "resources/shaders/floor_plane.fs");

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

		Cube cube(glm::vec3(10.0f, 3.0f, 0.0f), glm::vec3(0.75f));
		cube.init();

		Plane plane(glm::vec3(0.0f), glm::vec3(50.0f));
		plane.init();

		Model glock(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.05f));
		glock.loadModel("resources/models/Glock.fbx");

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		while (!Window::WindowShouldClose()) {
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			Window::ProcessInput(deltaTime);
			Window::PrepareFrame();

			// create transformation for screen
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = Camera::defaultCamera.getViewMatrix();
			projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);

			texturedObjectShader.activate();
			texturedObjectShader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
			texturedObjectShader.setMat4("view", view);
			texturedObjectShader.setMat4("projection", projection);

			/*	m.draw(shader);*/
			cube.draw(texturedObjectShader);

			plane.draw(texturedObjectShader);

			texturedObjectShader.activate();
			glock.draw(texturedObjectShader);

			cubemap.render(skyboxShader, Camera::defaultCamera.getViewMatrix(), projection);

			Window::ProcessEvents();
		}

		cube.cleanup();
		glock.cleanup();
		cubemap.cleanup();
		plane.cleanup();

		Window::ShutDown();
	}
}
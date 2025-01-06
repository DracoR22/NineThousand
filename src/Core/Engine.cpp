#include "Engine.h"

namespace Engine {
	void Run() {
		Window::Init();
		Physics::InitPhysx();

		float rotationAngle = 0.0f;
		
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 5.0f, 0.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);
		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));

		// shaders
		Shader texturedObjectShader("resources/shaders/textured_obj.vs", "resources/shaders/textured_obj.fs");
		Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
		Shader floorPlaneShader("resources/shaders/floor_plane.vs", "resources/shaders/floor_plane.fs");
		Shader animShader("resources/shaders/animated.vs", "resources/shaders/animated.fs");

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

		Cube cube(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.75f));
		cube.init();

		Plane plane(glm::vec3(0.0f), glm::vec3(50.0f));
		plane.init();

		Model glock(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.05f));
		glock.loadModel("resources/models/Glock.fbx");

		Animation glockIdleAnimation("resources/animations/Glock_Idle.fbx", &glock);
		Animator glockAnimator(&glockIdleAnimation);

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		while (!Window::WindowShouldClose()) {
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			// step the physics simulation
			Physics::Simulate(deltaTime);

			Window::ProcessInput(deltaTime);
			Window::PrepareFrame();

			glockAnimator.UpdateAnimation(deltaTime);

			// create transformation for screen
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = Camera::defaultCamera.getViewMatrix();
			projection = glm::perspective(glm::radians(Camera::defaultCamera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);

			texturedObjectShader.activate();
			texturedObjectShader.set3Float("viewPos", Camera::defaultCamera.cameraPos);
			texturedObjectShader.setMat4("view", view);
			texturedObjectShader.setMat4("projection", projection);

			// test rotation
			//rotationAngle += glm::radians(1.0f); // Increment the angle (1 degree per frame)
			//if (rotationAngle >= glm::two_pi<float>()) { // Reset after full rotation
			//	rotationAngle = 0.0f;
			//}
			//glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			/*	m.draw(shader);*/
			cube.draw(texturedObjectShader);
			cube.setRotation(rotationMatrix);
			cube.setPosition(cubeTransformData.position);

			/*cube.setPosition(glm::vec3(0.0f, 5.0f, 0.0f));
			cube.setRotation(glm::mat4(1.0f));*/

			plane.draw(texturedObjectShader);

			animShader.activate();
			animShader.setMat4("view", view);
			animShader.setMat4("projection", projection);
			auto transforms = glockAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

			glock.draw(animShader);

			cubemap.render(skyboxShader, Camera::defaultCamera.getViewMatrix(), projection);

			Window::ProcessEvents();
		}

		cube.cleanup();
		glock.cleanup();
		cubemap.cleanup();
		plane.cleanup();

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}
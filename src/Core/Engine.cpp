#include "Engine.h"

namespace Engine {
	void Run() {
		Window::Init();
		Physics::InitPhysx();

		glm::vec3 gunOffset(0.2f, -3.5f, 2.6f);

		Player player(glm::vec3(0.0f, 1.8f, 0.0f), 2.3f, 75.0f);

		float rotationAngle = 0.0f;
		
		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 10.0f, 1.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);

		// shaders
		Shader texturedObjectShader("resources/shaders/textured_obj.vs", "resources/shaders/textured_obj.fs");
		Shader skyboxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
		Shader animShader("resources/shaders/animated.vs", "resources/shaders/animated.fs");
		Shader stencilShader("resources/shaders/stencil_color.vs", "resources/shaders/stencil_color.fs");

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

		Cube cube(glm::vec3(0.0f, 5.0f, 1.0f), glm::vec3(0.75f));
		cube.init();

		Cube dCube(glm::vec3(0.0f, 5.0f, 1.0f), glm::vec3(0.75f));
		dCube.init();

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

			player.processInput(deltaTime);
			Window::ProcessInput(deltaTime);
			Window::PrepareFrame();

			glockAnimator.UpdateAnimation(deltaTime);

			// create transformation for screen
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);


			view = player.camera.getViewMatrix();
			projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.1f, 100.0f);


			glm::vec3 gunPosition = player.getPosition() +
				(player.camera.cameraFront * 0.7f) +   // Offset forward
				(player.camera.cameraUp * -3.85f);    // Offset downward

			// Calculate gun rotation to align with the camera
			glm::mat4 gunRotation = glm::mat4(1.0f);

			// Rotate around camera right (tilt up/down)
			float theta = acos(glm::dot(player.camera.worldUp, player.camera.cameraFront) /
				(glm::length(player.camera.worldUp) * glm::length(player.camera.cameraFront)));
			gunRotation = glm::rotate(gunRotation, glm::half_pi<float>() - theta, player.camera.cameraRight);

			// Rotate around camera up (align gun to camera facing direction in horizontal plane)
			glm::vec2 front2D = glm::vec2(player.camera.cameraFront.x, player.camera.cameraFront.z);
			theta = acos(glm::dot(glm::vec2(1.0f, 0.0f), glm::normalize(front2D)));
			gunRotation = glm::rotate(gunRotation, player.camera.cameraFront.z < 0 ? theta : -theta, player.camera.worldUp);

			// Apply local rotation adjustment to align gun's forward direction
			glm::mat4 localRotationFix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Adjust for left-facing gun
			localRotationFix = glm::rotate(localRotationFix, glm::radians(5.0f), glm::vec3(1.9f, 0.0f, 0.0f)); // Slight upward pitch adjustment
			gunRotation = gunRotation * localRotationFix;

			// Apply transformations to the gun
			glock.setPosition(gunPosition);
			glock.setRotation(gunRotation);

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			cube.setRotation(rotationMatrix);
			cube.setPosition(cubeTransformData.position);

			// Render Pipeline
			animShader.activate();
			animShader.setMat4("view", view);
			animShader.setMat4("projection", projection);
			auto transforms = glockAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

			glock.draw(animShader);

			texturedObjectShader.activate();
			texturedObjectShader.set3Float("viewPos", player.getPosition());
			texturedObjectShader.setMat4("view", view);
			texturedObjectShader.setMat4("projection", projection);

			plane.draw(texturedObjectShader);

			cube.draw(texturedObjectShader);

			cubemap.render(skyboxShader, player.camera.getViewMatrix(), projection);

			Window::ProcessEvents();
		}

		cube.cleanup();
		dCube.cleanup();
		glock.cleanup();
		cubemap.cleanup();
		plane.cleanup();

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}
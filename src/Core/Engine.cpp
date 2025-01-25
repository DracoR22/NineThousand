#include "Engine.h"

namespace Engine {
	void Run() {
		Window::Init();
		Physics::InitPhysx();

		Player player(glm::vec3(0.0f, 1.8f, 0.0f), 1.8f, 75.0f);

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

		Animation glockIdleAnimation("resources/animations/Glock_Reload.fbx", &glock);
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

			animShader.activate();
			animShader.setMat4("view", view);
			animShader.setMat4("projection", projection);
			auto transforms = glockAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);

			glStencilMask(0x00);
			glock.draw(animShader);

			texturedObjectShader.activate();
			texturedObjectShader.set3Float("viewPos", player.getPosition());
			texturedObjectShader.setMat4("view", view);
			texturedObjectShader.setMat4("projection", projection);

			PhysicsTransformData playerTransformData = Physics::GetTransformFromPhysics(player.actor);
			glm::mat4 playerRotationMatrix = glm::mat4_cast(playerTransformData.rotation);

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			plane.draw(texturedObjectShader);

			

			// 1fst render pass
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			cube.draw(texturedObjectShader);
			cube.setRotation(rotationMatrix);
			cube.setPosition(cubeTransformData.position);

			// 2nd render pass
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			glDisable(GL_DEPTH_TEST);

			stencilShader.activate();
			stencilShader.set3Float("viewPos", player.getPosition());
			stencilShader.setMat4("view", view);
			stencilShader.setMat4("projection", projection);

			float scale = 0.5f;
			dCube.setSize(glm::vec3(scale));
			dCube.draw(stencilShader);
			dCube.setRotation(rotationMatrix);
			dCube.setPosition(cubeTransformData.position);
		
			glStencilMask(0xFF);
			glStencilFunc(GL_ALWAYS, 0, 0xFF);
			glEnable(GL_DEPTH_TEST);

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
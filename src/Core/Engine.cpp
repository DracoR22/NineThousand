#include "Engine.h"

namespace Engine {

	struct Shaders {
		Shader texturedObjectShader;
		Shader skyboxShader;
		Shader animShader;
		Shader lampShader;
		Shader shadowMapShader;
		Shader groundShader;
	} _shaders;

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};


	void Run() {
		Window::Init();
		Physics::InitPhysx();

		glm::vec3 gunOffset(0.2f, -3.5f, 2.6f);
		glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

		Player player(glm::vec3(0.0f, 1.8f, 0.0f), 2.3f, 75.0f);

		float rotationAngle = 0.0f;
		
		physx::PxRigidStatic* planeActor = Physics::CreateStaticBox(physx::PxVec3(0.0, 0.0f, 0.0f), physx::PxVec3(50.0, 0.07f, 50.0f));
		physx::PxRigidDynamic* cubeActor = Physics::CreateDynamicBox(physx::PxVec3(0.0f, 10.0f, 1.0f), physx::PxVec3(0.75f, 0.75f, 0.75f), 10.0f);

		// load shaders
		_shaders.texturedObjectShader.load("textured_obj.vs", "textured_obj.fs");
		_shaders.skyboxShader.load("skybox.vs", "skybox.fs");
		_shaders.animShader.load("animated.vs", "animated.fs");
		_shaders.lampShader.load("lamp.vs", "lamp.fs");
		_shaders.shadowMapShader.load("shadow_map.vs", "shadow_map.fs");
		_shaders.groundShader.load("ground.vert", "ground.frag");

		// load skybox
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

		// load models
		Cube cube(glm::vec3(0.0f, 5.0f, 1.0f), glm::vec3(0.75f));
		cube.init();

		Cube cubeLamp(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.75f));
		cubeLamp.init();

		Cube cubeLamp2(glm::vec3(10.0f, 5.0f, 5.0f), glm::vec3(0.75f));
		cubeLamp2.init();

		Plane plane(glm::vec3(0.0f), glm::vec3(50.0f));
		plane.init();

		Model glock(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.05f));
		glock.loadModel("resources/models/Glock.fbx");

		std::vector<PointLight> sceneLights;

		PointLight cubeLampLight;
		cubeLampLight.position = cubeLamp.pos;
		cubeLampLight.constant = 1.0f;
		cubeLampLight.linear = 0.02f;
		cubeLampLight.quadratic = 0.01f;
		cubeLampLight.ambient = glm::vec3(0.3f);
		cubeLampLight.diffuse = glm::vec3(0.8f);
		cubeLampLight.specular = glm::vec3(1.0f);

		sceneLights.push_back(cubeLampLight);

		// Load shadow map
		ShadowMap shadowMap;
		shadowMap.Init();

		// load animations
		Animation glockIdleAnimation("resources/animations/Glock_Idle.fbx", &glock);
		Animator glockAnimator(&glockIdleAnimation);

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		while (!Window::WindowShouldClose()) {
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			Physics::Simulate(deltaTime);

			player.processInput(deltaTime);
			Window::ProcessInput(deltaTime);
			/*if (Keyboard::keyWentDown(GLFW_KEY_R)) {
				_shaders.texturedObjectShader.load("textured_obj.vs", "textured_obj.fs");
				_shaders.skyboxShader.load("skybox.vs", "skybox.fs");
				_shaders.animShader.load("animated.vs", "animated.fs");
				_shaders.lampShader.load("lamp.vs", "lamp.fs");
			}*/
			Window::PrepareFrame();

			glockAnimator.UpdateAnimation(deltaTime);

			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = player.camera.getViewMatrix();
			projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.5f, 50.0f);

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


			// ------ 1. SHADOW PASS (Render to Depth Map) ------
			glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
			glm::mat4 lightView = glm::lookAt(sceneLights[0].position, cube.pos, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProjection = orthogonalProjection * lightView;

			_shaders.shadowMapShader.activate();
			_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);

			shadowMap.Clear();

			glCullFace(GL_FRONT);
			cube.draw(_shaders.shadowMapShader);
			glCullFace(GL_BACK);

			shadowMap.Unbind();
			glViewport(0, 0, Window::currentWidth, Window::currentHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// ------ 2. RENDER PASS (Normal Rendering Map) ------
			_shaders.animShader.activate();
			_shaders.animShader.setMat4("view", view);
			_shaders.animShader.setMat4("projection", projection);
			auto transforms = glockAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			glock.draw(_shaders.animShader);

			_shaders.texturedObjectShader.activate();
			_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
			_shaders.texturedObjectShader.setMat4("view", view);
			_shaders.texturedObjectShader.setMat4("projection", projection);
			_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
			_shaders.texturedObjectShader.setInt("noPointLights", sceneLights.size());
			_shaders.texturedObjectShader.setInt("shadowMap", 2);
			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

				_shaders.texturedObjectShader.setVec3(lightUniform + ".position", sceneLights[i].position);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", sceneLights[i].constant);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", sceneLights[i].linear);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", sceneLights[i].quadratic);

				_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", sceneLights[i].ambient);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", sceneLights[i].diffuse);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", sceneLights[i].specular);
			}

			plane.draw(_shaders.texturedObjectShader);

			_shaders.texturedObjectShader.activate();
			_shaders.texturedObjectShader.set3Float("viewPos", player.getPosition());
			_shaders.texturedObjectShader.setMat4("view", view);
			_shaders.texturedObjectShader.setMat4("projection", projection);
			_shaders.texturedObjectShader.setMat4("lightProjection", lightProjection);
			_shaders.texturedObjectShader.setInt("noPointLights", sceneLights.size());
			_shaders.texturedObjectShader.setInt("shadowMap", 2);
			for (int i = 0; i < sceneLights.size(); i++) {
				std::string lightUniform = "pointLights[" + std::to_string(i) + "]";

				_shaders.texturedObjectShader.setVec3(lightUniform + ".position", sceneLights[i].position);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".constant", sceneLights[i].constant);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".linear", sceneLights[i].linear);
				_shaders.texturedObjectShader.setFloat(lightUniform + ".quadratic", sceneLights[i].quadratic);

				_shaders.texturedObjectShader.setVec3(lightUniform + ".ambient", sceneLights[i].ambient);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".diffuse", sceneLights[i].diffuse);
				_shaders.texturedObjectShader.setVec3(lightUniform + ".specular", sceneLights[i].specular);
			}
			cube.draw(_shaders.texturedObjectShader);

			_shaders.lampShader.activate();
			_shaders.lampShader.set3Float("viewPos", player.getPosition());
			_shaders.lampShader.setMat4("view", view);
			_shaders.lampShader.setMat4("projection", projection);
			_shaders.lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			cubeLamp.draw(_shaders.lampShader);

			cubemap.render(_shaders.skyboxShader, player.camera.getViewMatrix(), projection);

			Window::ProcessEvents();
		}

		shadowMap.Cleanup();
		cube.cleanup();
		glock.cleanup();
		cubeLamp.cleanup();
		cubemap.cleanup();
		plane.cleanup();

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}
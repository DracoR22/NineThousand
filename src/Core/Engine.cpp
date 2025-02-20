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
		Game::CreatePlayers();

		Player& player = Game::GetPLayerByIndex(0);

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
		ModelCreateInfo glockCreateInfo{
		 glm::vec3(0.0f, 3.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo p90CreateInfo{
		 glm::vec3(0.0f, 5.0f, 0.0f),
		 glm::vec3(0.05f),
		 glm::mat4(1.0f)
		};

		ModelCreateInfo cubeCreateInfo{
			glm::vec3(0.0f, 5.0f, 1.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo lampCreateInfo{
			glm::vec3(10.0f, 5.0f, 5.0f),
			glm::vec3(0.75f),
			glm::mat4(1.0f)
		};

		ModelCreateInfo planeCreateInfo{
			glm::vec3(0.0f),
			glm::vec3(50.0f),
			glm::mat4(1.0f)
		};

		AssetManager::LoadAssimpModel("P90", "resources/models/P90.fbx", p90CreateInfo);
		AssetManager::LoadAssimpModel("Glock", "resources/models/Glock.fbx", glockCreateInfo);

		AssetManager::LoadModel("Cube", ModelType::CUBE, cubeCreateInfo);
		AssetManager::LoadModel("CubeLamp", ModelType::CUBE, lampCreateInfo);
		AssetManager::LoadModel("Plane", ModelType::PLANE, planeCreateInfo);

		std::vector<PointLight> sceneLights;

		PointLight cubeLampLight;
		cubeLampLight.position = AssetManager::GetModelByName("CubeLamp")->pos;
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

		Model* glockModel = AssetManager::GetModelByName("Glock");
		Model* p90Model = AssetManager::GetModelByName("P90");

		// load animations
		AssetManager::LoadAnimation("GlockIdle", "resources/animations/Glock_Idle.fbx", glockModel);
		Animator glockAnimator(AssetManager::GetAnimationByName("GlockIdle"));

		AssetManager::LoadAnimation("P90Idle", "resources/animations/P90_Idle.fbx", p90Model);
		Animator p90Animator(AssetManager::GetAnimationByName("P90Idle"));

		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		double lastTime = glfwGetTime();
		int nbFrames = 0;

		while (!Window::WindowShouldClose()) {
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			nbFrames++;
			if (currentTime - lastTime >= 1.0) { // If a second has passed
				// Print and reset FPS counter
				std::cout << "FPS: " << nbFrames << std::endl;
				nbFrames = 0;
				lastTime += 1.0;
			}


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
			/*p90Animator.UpdateAnimation(deltaTime);*/

			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = player.camera.getViewMatrix();
			projection = glm::perspective(glm::radians(player.camera.getZoom()), (float)Window::currentWidth / (float)Window::currentHeight, 0.5f, 50.0f);

			Game::Update();

			PhysicsTransformData cubeTransformData = Physics::GetTransformFromPhysics(cubeActor);
			glm::mat4 rotationMatrix = glm::mat4_cast(cubeTransformData.rotation);

			AssetManager::GetModelByName("Cube")->setPosition(cubeTransformData.position);
			AssetManager::GetModelByName("Cube")->setRotation(rotationMatrix);

			AssetManager::GetModelByName("P90")->setPosition(glm::vec3(5.0f, 5.0f, 5.0f));


			// ------ 1. SHADOW PASS (Render to Depth Map) ------
			glm::mat4 orthogonalProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
			glm::mat4 lightView = glm::lookAt(sceneLights[0].position, AssetManager::GetModelByName("Cube")->pos, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProjection = orthogonalProjection * lightView;

			shadowMap.Clear();

			glCullFace(GL_FRONT);

			_shaders.shadowMapShader.activate();
			_shaders.shadowMapShader.setMat4("lightProjection", lightProjection);
			AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

			glCullFace(GL_BACK);

			shadowMap.Unbind();
			glViewport(0, 0, Window::currentWidth, Window::currentHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


			// ------ 2. RENDER PASS ------
			_shaders.animShader.activate();
			_shaders.animShader.setMat4("view", view);
			_shaders.animShader.setMat4("projection", projection);
			auto transforms = glockAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < transforms.size(); ++i)
				_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
			AssetManager::DrawModel("Glock", _shaders.animShader);

		/*	_shaders.animShader.activate();
			_shaders.animShader.setMat4("view", view);
			_shaders.animShader.setMat4("projection", projection);
			auto p90Transforms = p90Animator.GetFinalBoneMatrices();
			for (int i = 0; i < p90Transforms.size(); ++i)
				_shaders.animShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", p90Transforms[i]);
			AssetManager::DrawModel("P90", _shaders.animShader);*/

			/*_shaders.texturedObjectShader.activate();
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
			AssetManager::DrawModel("P90", _shaders.texturedObjectShader);*/

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

			/*Scene::GetPrimitiveModelByName("Plane")->draw(_shaders.texturedObjectShader);*/
			AssetManager::DrawModel("Plane", _shaders.texturedObjectShader);

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
			/*Scene::GetPrimitiveModelByName("Cube")->draw(_shaders.texturedObjectShader);*/
			AssetManager::DrawModel("Cube", _shaders.shadowMapShader);

			_shaders.lampShader.activate();
			_shaders.lampShader.set3Float("viewPos", player.getPosition());
			_shaders.lampShader.setMat4("view", view);
			_shaders.lampShader.setMat4("projection", projection);
			_shaders.lampShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			/*Scene::GetPrimitiveModelByName("CubeLamp")->draw(_shaders.lampShader);*/
			AssetManager::DrawModel("CubeLamp", _shaders.lampShader);

			cubemap.render(_shaders.skyboxShader, player.camera.getViewMatrix(), projection);

			Window::ProcessEvents();
		}

		shadowMap.Cleanup();
		AssetManager::CleanupModels();
		cubemap.cleanup();
		/*Scene::GetPrimitiveModelByName("Plane")->cleanup();*/

		Physics::CleanupPhysX();

		Window::ShutDown();
	}
}
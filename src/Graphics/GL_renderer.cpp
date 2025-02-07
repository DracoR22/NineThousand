#include "GL_renderer.h"

namespace OpenGLRenderer {

	struct Shaders {
		Shader texturedObjectShader;
		Shader skyboxShader;
		Shader animShader;
		Shader lampShader;
		Shader shadowMapShader;
		Shader groundShader;
	} _shaders;

	struct Models {
		PrimitiveModel cube;
		PrimitiveModel cubeLamp;
		PrimitiveModel cubeLamp2;
		PrimitiveModel plane;

		Model glock;

	} g_models;

	struct PointLight {
		glm::vec3 position;

		float constant;
		float linear;
		float quadratic;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
	};

	void Init() {
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
		g_models.cube = PrimitiveModel(PrimitiveModel::Type::CUBE, glm::vec3(0.0f, 5.0f, 1.0f), glm::vec3(0.75f));
		g_models.cube.Init();

		g_models.cubeLamp = PrimitiveModel(PrimitiveModel::Type::CUBE, glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.75f));
		g_models.cubeLamp.Init();

		g_models.cubeLamp2 = PrimitiveModel(PrimitiveModel::Type::CUBE, glm::vec3(10.0f, 5.0f, 5.0f), glm::vec3(0.75f));
		g_models.cubeLamp2.Init();

		g_models.plane = PrimitiveModel(PrimitiveModel::Type::PLANE, glm::vec3(0.0f), glm::vec3(50.0f));
		g_models.plane.Init();

		g_models.glock = Model(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.05f));
		g_models.glock.loadModel("resources/models/Glock.fbx");

		std::vector<PointLight> sceneLights;

		PointLight cubeLampLight;
		cubeLampLight.position = g_models.cubeLamp.pos;
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
		Animation glockIdleAnimation("resources/animations/Glock_Idle.fbx", &g_models.glock);
		Animator glockAnimator(&glockIdleAnimation);
	}

	void RenderFrame() {

	}
}
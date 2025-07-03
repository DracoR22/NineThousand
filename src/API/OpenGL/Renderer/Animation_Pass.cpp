#include "./Renderer.h"

void OpenGLRenderer::AnimationPass() {
	Shader* animationShader = GetShaderByName("Animation");

	Player& player = Game::GetPLayerByIndex(0);
	Camera* camera = CameraManager::GetActiveCamera();
	std::vector<LightCreateInfo>& sceneLights = GetSceneLights();

	animationShader->activate();
	animationShader->setMat4("view", camera->GetViewMatrix());
	animationShader->setMat4("projection", camera->GetProjectionMatrix());
	animationShader->setInt("noLights", sceneLights.size());
	for (int i = 0; i < sceneLights.size(); i++) {
		std::string lightUniform = "lights[" + std::to_string(i) + "]";

		animationShader->setFloat(lightUniform + ".posX", sceneLights[i].position.x);
		animationShader->setFloat(lightUniform + ".posY", sceneLights[i].position.y);
		animationShader->setFloat(lightUniform + ".posZ", sceneLights[i].position.z);
		animationShader->setFloat(lightUniform + ".radius", sceneLights[i].radius);
		animationShader->setFloat(lightUniform + ".strength", sceneLights[i].strength);
		animationShader->setFloat(lightUniform + ".colorR", sceneLights[i].color.r);
		animationShader->setFloat(lightUniform + ".colorG", sceneLights[i].color.g);
		animationShader->setFloat(lightUniform + ".colorB", sceneLights[i].color.b);
		animationShader->setInt(lightUniform + ".type", static_cast<int>(sceneLights[i].type));
	}
	animationShader->set3Float("camPos", CameraManager::GetActiveCamera()->cameraPos);

	glm::mat4 amodel = glm::mat4(1.0f);
	amodel = glm::translate(amodel, player.m_currentWeaponGameObject.GetPosition());
	amodel = glm::scale(amodel, player.m_currentWeaponGameObject.GetSize());
	amodel *= player.m_currentWeaponGameObject.GetRotationMatrix();
	animationShader->setMat4("model", amodel);
	animationShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(amodel))));

	auto& transforms = AssetManager::GetAnimatorByName(player.GetEquipedWeaponInfo()->name + "Animator")->GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i) {
		animationShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	AssetManager::DrawModel(player.GetEquipedWeaponInfo()->name, *animationShader);
}
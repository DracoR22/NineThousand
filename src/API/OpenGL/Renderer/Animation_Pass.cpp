#include "./Renderer.h"

void OpenGLRenderer::AnimationPass() {
	Shader* animationShader = GetShaderByName("Animation");

	Player& player = Game::GetPLayerByIndex(0);
	Camera* camera = CameraManager::GetActiveCamera();
	std::vector<LightCreateInfo>& sceneLights = GetSceneLights();

	Model* currentWeaponModel = AssetManager::GetModelByName(player.GetEquipedWeaponInfo()->name);
	WeaponInfo* currentWeaponInfo = player.GetEquipedWeaponInfo();

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

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, player.m_currentWeaponGameObject.GetPosition());
	model = glm::scale(model, player.m_currentWeaponGameObject.GetSize());
	model *= player.m_currentWeaponGameObject.GetRotationMatrix();
	animationShader->setMat4("model", model);
	animationShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));

	auto& transforms = AssetManager::GetAnimatorByName(currentWeaponInfo->name + "Animator")->GetFinalBoneMatrices();
	for (int i = 0; i < transforms.size(); ++i) {
		animationShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	}
	
	for (Mesh& mesh : currentWeaponModel->m_meshes) {
		std::string materialName = "Default";
		auto it = currentWeaponInfo->meshMaterials.find(mesh.m_Name);
		if (it != currentWeaponInfo->meshMaterials.end()) {
			materialName = it->second;
		}

		int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
		Material* meshMaterial = AssetManager::GetMaterialByIndex(materialIndex);
		Texture* baseTexture = AssetManager::GetTextureByIndex(meshMaterial->baseTexture);
		Texture* normalTexture = AssetManager::GetTextureByIndex(meshMaterial->normalTexture);
		Texture* rmaTexture = AssetManager::GetTextureByIndex(meshMaterial->rmaTexture);

		glActiveTexture(GL_TEXTURE0);
		animationShader->setInt("baseTexture", 0);
		baseTexture->Bind();

		glActiveTexture(GL_TEXTURE1);
		animationShader->setInt("normalTexture", 1);
		normalTexture->Bind();

		glActiveTexture(GL_TEXTURE2);
		animationShader->setInt("rmaTexture", 2);
		rmaTexture->Bind();

		glBindVertexArray(mesh.GetVAO());
		glDrawElements(GL_TRIANGLES, mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
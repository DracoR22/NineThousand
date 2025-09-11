#include "./Renderer.h"

void OpenGLRenderer::BillboardPass() {
	// ------ MUZZLE FLASH 
	Player& player = Game::GetPLayerByIndex(0);

	Camera* camera = CameraManager::GetActiveCamera();
	Shader* muzzleFlashShader = GetShaderByName("MuzzleFlash");

	Mesh2D* quadMesh = GetQuadMeshByName("Texture");

	glm::vec3 barrelOffset = player.IsInADS() ? player.GetEquipedWeaponInfo()->ADSMuzzleFlashOffset : player.GetEquipedWeaponInfo()->muzzleFlashOffset;
	glm::vec3 muzzleFlashSize = player.GetEquipedWeaponInfo()->muzzleFlashSize;

	glm::mat4 gunTransform = glm::translate(glm::mat4(1.0f), player.m_currentWeaponGameObject.GetPosition()) * player.m_currentWeaponGameObject.GetRotationMatrix();
	glm::vec4 worldBarrelPos = gunTransform * glm::vec4(barrelOffset, 1.0f);

	glm::mat4 muzzleFlashModel = glm::mat4(1.0f);

	muzzleFlashModel = glm::translate(muzzleFlashModel, glm::vec3(worldBarrelPos));
	muzzleFlashModel *= glm::inverse(glm::mat4(glm::mat3(camera->GetViewMatrix())));
	muzzleFlashModel = glm::scale(muzzleFlashModel, muzzleFlashSize);

	float randomAngle = Utils::RandomFloat(0.0f, glm::two_pi<float>());

	muzzleFlashShader->activate();

	muzzleFlashShader->setMat4("model", muzzleFlashModel);
	muzzleFlashShader->setMat4("view", camera->GetViewMatrix());
	muzzleFlashShader->setMat4("projection", camera->GetProjectionMatrix());
	muzzleFlashShader->setFloat("rotation", 0.0f);

	Texture* muzzleFlashTexture = AssetManager::GetTextureByName("MuzzleFlash.png");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, muzzleFlashTexture->m_id);
	muzzleFlashShader->setInt("baseTexture", 0);

	if (player.m_muzzleFlashTimer > 0) {
		quadMesh->RenderTexture(*muzzleFlashShader);
		player.m_muzzleFlashTimer--;
	}
}
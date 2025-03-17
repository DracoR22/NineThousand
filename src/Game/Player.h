#pragma once

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "WeaponManager.h"

#include "../Core/AssetManager.h"

struct PlayerState {
	bool isDrawingWeapon = false;
	bool isReloadingWeapon = false;
};

class Player {
public:
	bool m_isOnGround = true;

private:
	bool m_isMoving = false;

	glm::vec3 velocity;
	float speed;
	float height;
	float verticalVelocity = 0.0f;

	WeaponInfo* m_equippedWeapon = nullptr;

public:
	std::vector<BulletInfo> spawnedBullets = {};
public:
	Player(glm::vec3 position, float height, float mass);

	void Update(float deltaTime);

    void processInput(double deltaTime);

	Camera camera;

	glm::vec3 getPosition();

	bool CanReload();
	bool IsMoving();

	void EquipWeapon(std::string weaponName);
	WeaponInfo* GetEquipedWeaponInfo();

	void ReloadWeapon();
	void FireWeapon();
};
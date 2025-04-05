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
private:
	bool m_isOnGround = true;
	bool m_isMoving = false;
	bool m_isAimingWeapon = false;

	glm::vec3 m_velocity;

	float m_speed = 0.0f;
	float m_height = 0.0f;

	WeaponInfo* m_equippedWeapon = nullptr;
public:
	Player(glm::vec3 position, float height, float mass);

	void Update(float deltaTime);
	void MoveCharacterController(double deltaTime);

    void processInput(double deltaTime);

	Camera camera;

	glm::vec3 getPosition();

	bool CanReload();
	bool IsMoving();

	void EquipWeapon(std::string weaponName);
	WeaponInfo* GetEquipedWeaponInfo();

	void ReloadWeapon();
	void FireWeapon();
	void EnterADS();
	void LeaveADS();
	void IsInADS();
};
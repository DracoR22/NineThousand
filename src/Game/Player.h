#pragma once

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "WeaponManager.h"

#include "../Core/AssetManager.h"


class Player {
private:
	bool m_isOnGround = true;
	bool m_isMoving = false;
	bool m_isAimingWeapon = false;
	
	glm::vec3 m_velocity = glm::vec3(0.0f);

	float m_speed = 0.0f;
	float m_height = 0.0f;

	WeaponInfo* m_equippedWeapon = nullptr;
	WeaponAction m_weaponAction = WeaponAction::IDLE;
public:
	Player(glm::vec3 position, float height, float mass);

    void processInput(double deltaTime);

	Camera camera;

	glm::vec3 getPosition();

	WeaponInfo* GetEquipedWeaponInfo();

	bool IsMoving();

	bool CanReloadWeapon();
	bool CanEnterADS();
	bool CanFireWeapon();
	bool IsDrawingWeapon();

	bool PressingFire();
	bool PressingADS();
	bool PressedADS();
	bool PressedReload();
	bool ReleasedADS();
	bool IsInADS();

	void ReloadWeapon();
	void FireWeapon();
	void EnterADS();
	void LeaveADS();
	void EquipWeapon(std::string weaponName);
	
	void UpdateWeaponLogic();

	WeaponAction GetWeaponAction();
	void SetWeaponAction(WeaponAction action);
};
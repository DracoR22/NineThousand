#pragma once

#include "../Camera/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Audio/AudioManager.h"

#include "WeaponManager.h"

#include "../Core/AssetManager.h"
#include "../Objects/GameObject.h"


class Player {
private:
	bool m_isOnGround = true;
	bool m_isMoving = false;
	bool m_isAimingWeapon = false;
	
	float m_speed = 0.0f;
	float m_height = 0.0f;

	WeaponInfo* m_equippedWeapon = nullptr;
	WeaponAction m_weaponAction = WeaponAction::IDLE;
	std::unordered_map<std::string, WeaponState> m_weaponStates;
public:
	Player() = default;
	Player(glm::vec3 position, float height, float mass);

    void Update(double deltaTime);

	glm::vec3 getPosition();
	bool IsMoving();

	void InitWeaponStates();

	WeaponInfo* GetEquipedWeaponInfo();
	WeaponState* GetEquipedWeaponState();

	bool CanReloadWeapon();	
	bool CanAutoReloadWeapon();
	bool CanEnterADS();
	bool CanFireWeapon();
	bool IsDrawingWeapon();  // TODO

	bool PressingFire();
	bool PressedFire();
	bool PressingADS();
	bool PressedADS();
	bool PressedReload();
	bool ReleasedADS();
	bool IsInADS();

	void ReloadWeapon();
	void FireWeapon();
	void MeleeHit();
	void EnterADS();
	void LeaveADS();

	void EquipWeapon(std::string weaponName);
	
	void UpdateWeaponLogic();

	WeaponAction GetWeaponAction();
	void SetWeaponAction(WeaponAction action);
public:
	Camera m_camera;
	float m_muzzleFlashTimer = 0;
	GameObject m_currentWeaponGameObject;
};
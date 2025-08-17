#pragma once

#include "../Camera/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Audio/AudioManager.h"

#include "WeaponManager.h"
#include "./Scene.h"

#include "../Core/AssetManager.h"
#include "../Objects/GameObject.h"
#include "../Objects/WaterObject.h"


class Player {
public:
	Player() = default;
	Player(glm::vec3 position, float height);

    void Update(double deltaTime);
	void UpdateMovement(double deltaTime);
	void UpdateAudio(double deltaTime);

	glm::vec3 getPosition();
	bool IsMoving();
	bool IsSprinting();

	void InitWeaponStates();

	WeaponInfo* GetEquipedWeaponInfo();
	WeaponState* GetEquipedWeaponState();
	WeaponAction GetWeaponAction();
	std::string GetStringWeaponAction();

	bool CanReloadWeapon();	
	bool CanAutoReloadWeapon();
	bool CanEnterADS();
	bool CanFireWeapon();
	bool IsDrawingWeapon();  // TODO

	bool PressingSprintKey();
	bool PressingFire();
	bool PressedFire();
	bool PressingADS();
	bool PressedADS();
	bool PressedReload();
	bool ReleasedADS();
	bool ReleasedFire();
	bool IsInADS();

	void ReloadWeapon();
	void FireWeapon();
	void MeleeHit();
	void EnterADS();
	void LeaveADS();

	void EquipWeapon(std::string weaponName);
	
	void UpdateWeaponLogic();

	void SetWeaponAction(WeaponAction action);

	void SpawnBulletCase();
public:
	Camera m_camera;
	float m_muzzleFlashTimer = 0;
	GameObject m_currentWeaponGameObject;
	uint64_t m_bulletCasePhysicsId = -1;
private:
	uint64_t m_physicsId;

	bool m_isOnGround = true;
	bool m_isJumping = false;
	bool m_isMoving = false;
	bool m_isSprinting = false;
	bool m_isAimingWeapon = false;
	bool m_isOnWaterSurface = false;

	float m_bulletCaseSoundCooldown = 0.0f;

	glm::vec3 m_position;
	float m_speed = 0.0f;
	float m_height = 0.0f;
	float m_mass = 75.0f;

	WeaponInfo* m_equippedWeapon = nullptr;
	WeaponAction m_weaponAction = WeaponAction::IDLE;
	std::unordered_map<std::string, WeaponState> m_weaponStates;
};
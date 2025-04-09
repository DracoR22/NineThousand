#pragma once

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "WeaponManager.h"

#include "../Core/AssetManager.h"

class Player {
public: 
	// animation timers
	float m_ADSFireAnimationFinishTime = 0.0f;
	float m_ADSInAnimationFinishTime = 0.0f;
private:
	bool m_isOnGround = true;
	bool m_isMoving = false;
	bool m_isAimingWeapon = false;
	
	glm::vec3 m_velocity;

	float m_speed = 0.0f;
	float m_height = 0.0f;

	WeaponInfo* m_equippedWeapon = nullptr;
	WeaponAction m_weaponAction = WeaponAction::IDLE;
public:
	Player(glm::vec3 position, float height, float mass);

	void Update(float deltaTime);
	void MoveCharacterController(double deltaTime);

    void processInput(double deltaTime);

	Camera camera;

	glm::vec3 getPosition();

	bool IsMoving();

	bool CanReload();
	bool IsDrawingWeapon();

	WeaponInfo* GetEquipedWeaponInfo();
	WeaponAction GetWeaponAction();

	bool PressingADS();
	bool PressedADS();
	bool ReleasedADS();

	void ReloadWeapon();
	void FireWeapon();
	void EnterADS();
	void LeaveADS();
	void EquipWeapon(std::string weaponName);

	void SetWeaponAction(WeaponAction action);
};
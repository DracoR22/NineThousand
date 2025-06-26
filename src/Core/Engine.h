#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../UI/Text2D.h"
#include "../UI/UIManager.h"
#include "../Editor/EditorPanel.h"
#include "../Editor/Editor.h"

#include "../Core/Window.h"
#include "../Core/AssetManager.h"

#include "../API/OpenGL/Renderer/Renderer.h"

#include "../Camera/Camera.h"
#include "../Camera/CameraManager.h"

#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Animation/Animation.hpp"
#include "../Animation/Animator.hpp"

#include "../Game/Player.h"
#include "../Game/Scene.h"
#include "../Game/Game.h"
#include "../Game/WeaponManager.h"

#include "../Audio/AudioManager.h"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include <stb_image.h>
#include <assimp/version.h>

namespace Engine {
	void Run();
	void LoadResources();
}
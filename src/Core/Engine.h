#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Core/Window.h"
#include "../Core/AssetManager.h"


#include "../API/OpenGL/Shader.h"
#include "../API/OpenGL/Cubemap.h"
#include "../API/OpenGL/ShadowMap.h"
#include "../API/OpenGL/FrameBuffer.hpp"
#include "../API/OpenGL/Renderer.h"

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Animations/Animation.hpp"
#include "../Animations/Animator.hpp"

#include "../Game/Player.h"
#include "../Game/Scene.h"
#include "../Game/Game.h"
#include "../Game/WeaponManager.h"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include <stb_image.h>

namespace Engine {
	void Run();
}
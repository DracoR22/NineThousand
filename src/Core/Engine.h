#pragma once

#include <iostream>

#include "../Core/Window.h"

#include "../Graphics/Shader.h"
#include "../Graphics/Primitives.hpp"
#include "../Graphics/Cubemap.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Animations/Animation.hpp"
#include "../Animations/Animator.hpp"

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include <stb_image.h>

namespace Engine {
	void Run();
}
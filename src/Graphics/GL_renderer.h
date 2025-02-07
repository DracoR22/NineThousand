#pragma once

#include "Shader.h"
#include "Primitives.hpp"
#include "Cubemap.h"
#include "ShadowMap.h"

#include "../Input/Camera.h"
#include "../Input/Keyboard.h"
#include "../Input/Mouse.h"

#include "../Physics/Physics.h"

#include "../Animations/Animation.hpp"
#include "../Animations/Animator.hpp"

#include "../Game/Player.h"

namespace OpenGLRenderer {
	void Init();
	void RenderFrame();
}
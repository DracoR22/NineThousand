#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>

#include <fmod.hpp>
#include <fmod_errors.h>

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace AudioManager {
	void Init();
	void Update();
	void LoadAudio(const std::string& filename);
	void PlayAudio(const std::string& filename, float volume, float frequency);
	void Cleanup(); 
	bool SucceededOrWarn(const std::string& message, FMOD_RESULT result);
}
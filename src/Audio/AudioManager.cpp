#include "AudioManager.h"

namespace AudioManager {
	std::unordered_map<std::string, FMOD::Sound*> g_loadedAudio;
	FMOD::System* g_system;
	FMOD::ChannelGroup* g_channelGroup = nullptr;

	void Init() {
		FMOD_RESULT result = FMOD::System_Create(&g_system);
		if (!SucceededOrWarn("AudioManager::Init(): Failed to create system object", result)) {
			return;
		}

		result = g_system->init(512, FMOD_INIT_NORMAL, nullptr);
		if (!SucceededOrWarn("AudioManager::Init(): Failed to create system object", result)) {
			return;
		}

		result = g_system->createChannelGroup("channels", &g_channelGroup);
		if (!SucceededOrWarn("AudioManager::Init(): Failed to create system object", result)) {
			return;
		}
	}

	void Update() {
		if (g_system) {
			g_system->update();
		}
	}

	void LoadAudio(const std::string& filename) {
		FMOD::Sound* sound = nullptr;
		FMOD_RESULT result = g_system->createSound(("resources/audio/" + filename).c_str(), FMOD_DEFAULT, nullptr, &sound);

		if (!SucceededOrWarn("AudioManager::LoadAudio(): Failed to load sound: " + filename, result)) {
			return;
		}

		g_loadedAudio[filename] = sound;
	}

	void PlayAudio(const std::string& filename, float volume, float frequency) {
		auto it = g_loadedAudio.find(filename);
		if (it == g_loadedAudio.end()) {
			SucceededOrWarn("FMOD: Sound not found: " + filename, FMOD_ERR_INVALID_HANDLE);
			return;
		}

		FMOD::Channel* channel = nullptr;
		FMOD_RESULT result = g_system->playSound(it->second, nullptr, false, &channel);
		if (!SucceededOrWarn("FMOD: Failed to play sound: " + filename, result)) {
			return;
		}

		if (channel) {
			channel->setVolume(volume);
			channel->setChannelGroup(g_channelGroup);
			float currentFrequency = 0.0f;
			channel->getFrequency(&currentFrequency);
			channel->setFrequency(currentFrequency * frequency);
		}
	}

	void Cleanup() {
		for (auto& [name, sound] : g_loadedAudio) {
			sound->release();
		}
		g_loadedAudio.clear();

		if (g_channelGroup) g_channelGroup->release();
		if (g_system) {
			g_system->close();
			g_system->release();
		}
	}

	bool SucceededOrWarn(const std::string& message, FMOD_RESULT result) {
		if (result != FMOD_OK) {
			std::cerr << message << ": " << result << " " << FMOD_ErrorString(result) << std::endl;
			return false;
		}
		return true;
	}
}
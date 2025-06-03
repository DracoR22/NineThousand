#include "JSON.h"

namespace JSON {
	void SaveToFile(nlohmann::json& json, const std::string& filePath) {
		std::ofstream ofs(filePath);
		if (!ofs.is_open()) {
			std::cout << "Failed to open file: " << filePath << std::endl;
		}
		ofs << json.dump(4);
		ofs.close();
		std::cout << "File Saved!" << std::endl;
	}

	bool LoadFile(nlohmann::json& json, const std::string filePath) {
		std::ifstream file(filePath);
		if (!file) {
			std::cout << "JSON::LoadFile() failed to open file: " << filePath << "\n";
			return false;
		}

		try {
			file >> json;
			return true;
		}
		catch (const nlohmann::json::parse_error& e) {
			std::cerr << "JSON::LoadFile() parse error in file " << filePath << ": " << e.what() << std::endl;
			return false;
		}
	  }
	
	void SaveLevel(const std::string& filePath, LevelCreateInfo& levelCreateInfo) {
		nlohmann::json json;

		json["LevelName"] = levelCreateInfo.name;
		json["GameObjects"] = nlohmann::json::array();

		for (const GameObjectCreateInfo& createInfo : levelCreateInfo.gameObjects) {
			json["GameObjects"].push_back(nlohmann::json{
				{ "name", createInfo.name },
				{ "modelName", createInfo.modelName },
				{ "position", createInfo.position },
				{ "size", createInfo.size },
				{ "rotation", createInfo.rotation }
				});
		}

		SaveToFile(json, filePath);
	}

	LevelCreateInfo LoadLevel(const std::string& filePath) {
		LevelCreateInfo levelCreateInfo;

		nlohmann::json json;
		if (!LoadFile(json, filePath)) {
			std::cerr << "JSON::LoadLevel() failed to open file: " << filePath << "\n";
			return levelCreateInfo;
		}

		levelCreateInfo.name = std::filesystem::path(filePath).filename().string();

		for (auto& jsonObject : json["GameObjects"]) {
			GameObjectCreateInfo& createInfo = levelCreateInfo.gameObjects.emplace_back();
			createInfo.name = jsonObject["name"];
			createInfo.modelName = jsonObject["modelName"];
			createInfo.position = jsonObject["position"];
			createInfo.rotation = jsonObject["rotation"];
			createInfo.size = jsonObject["size"];
		}

		return levelCreateInfo;
	}
}
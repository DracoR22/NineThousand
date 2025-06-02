#include "JSON.h"
//
//namespace nlohmann {
//
//	void to_json(nlohmann::json& j, const GameObjectCreateInfo& info) {
//		j = nlohmann::json{
//		  {"name", info.name},
//		};
//	}
//
//	void from_json(const nlohmann::json& j, GameObjectCreateInfo& info) {
//		j.at("name").get_to(info.name);
//	}
//
//
//}
//


namespace JSON {
	void SaveLevel(const std::string& fileName, LevelCreateInfo& levelCreateInfo) {
		nlohmann::json json;

		json["LevelName"] = levelCreateInfo.name;
		json["GameObjects"] = nlohmann::json::array();

		for (const GameObjectCreateInfo& createInfo : levelCreateInfo.gameObjects) {
			json["GameObjects"].push_back(nlohmann::json{
				{ "name", createInfo.name },
				{ "modelName", createInfo.modelName },
				{ "position", createInfo.position }
			});
		}

		SaveToFile(json, fileName);
	}

	void SaveToFile(nlohmann::json& json, const std::string& fileName) {
		std::ofstream ofs(fileName);
		if (!ofs.is_open()) {
			std::cout << "Failed to open file: " << fileName << std::endl;
		}
		ofs << json.dump(4);
		ofs.close();
		std::cout << "File Saved!" << std::endl;
	}
}
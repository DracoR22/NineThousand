#include "JSON.h"

#include "../Core/AssetManager.h"

namespace nlohmann {
	void adl_serializer<glm::vec3>::to_json(nlohmann::json& j, const glm::vec3& value) {
		j = nlohmann::json({
			{"x", value.x},
			{"y", value.y},
			{"z", value.z}
			});
	}

	void adl_serializer<glm::vec3>::from_json(const nlohmann::json& j, glm::vec3& value) {
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
		j.at("z").get_to(value.z);
	}

	void adl_serializer<glm::vec2>::to_json(nlohmann::json& j, const glm::vec2& value) {
		j = nlohmann::json({
			{"x", value.x},
			{"y", value.y}
			});
	}

	void adl_serializer<glm::vec2>::from_json(const nlohmann::json& j, glm::vec2& value) {
		j.at("x").get_to(value.x);
		j.at("y").get_to(value.y);
	}

	void adl_serializer<MeshRenderingInfo>::to_json(nlohmann::json& j, const MeshRenderingInfo& value) {
		Material* material = AssetManager::GetMaterialByIndex(value.materialIndex);
		Mesh* mesh = AssetManager::GetMeshByIndex(value.meshIndex);

		if (!material) return;
		if (!mesh) return;

		j = nlohmann::json{
	       {"meshName", mesh->m_Name},
	       {"materialName", material->name}
		};
	}

	void adl_serializer<MeshRenderingInfo>::from_json(const nlohmann::json& j, MeshRenderingInfo& value) {
		std::string meshName;
		std::string materialName;

		j.at("meshName").get_to(meshName);
		j.at("materialName").get_to(materialName);

		value.meshIndex = AssetManager::GetMeshIndexByName(meshName);
		value.materialIndex = AssetManager::GetMaterialIndexByName(materialName);
	}

	void adl_serializer<GameObjectCreateInfo>::to_json(nlohmann::json& j, const GameObjectCreateInfo& obj) {
		j = {
			{"name", obj.name},
			{"modelName", obj.modelName},
			{"position", obj.position},
			{"rotation", obj.rotation},
			{"textureScale", obj.textureScale},
			{"meshRenderingInfo", obj.meshRenderingInfo}
		};
	}

	void adl_serializer<GameObjectCreateInfo>::from_json(const nlohmann::json& j, GameObjectCreateInfo& obj) {
		j.at("name").get_to(obj.name);
		j.at("modelName").get_to(obj.modelName);
		j.at("position").get_to(obj.position);
		j.at("rotation").get_to(obj.rotation);
		j.at("textureScale").get_to(obj.textureScale);
	}
}

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
				{ "rotation", createInfo.rotation },
		    	{ "textureScale", createInfo.textureScale },
				{ "meshRenderingInfo", createInfo.meshRenderingInfo }
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
			createInfo.textureScale = jsonObject["textureScale"];
			createInfo.meshRenderingInfo = jsonObject["meshRenderingInfo"];
		}

		return levelCreateInfo;
	}
}
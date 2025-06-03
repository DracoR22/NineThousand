#pragma once

#include <iostream>
#include <fstream> 
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "../Common/CreateInfo.hpp"

namespace nlohmann {
    template <>
    struct adl_serializer<glm::vec3> {
        static void to_json(nlohmann::json& j, const glm::vec3& value) {
            j = nlohmann::json({
                {"x", value.x},
                {"y", value.y},
                {"z", value.z}
             });
        }

        static void from_json(const nlohmann::json& j, glm::vec3& value) {
            j.at("x").get_to(value.x);
            j.at("y").get_to(value.y);
            j.at("z").get_to(value.z);
        }
    };

    template <>
    struct adl_serializer<GameObjectCreateInfo> {
        static void to_json(nlohmann::json& j, const GameObjectCreateInfo& obj) {
            j = {
                {"name", obj.name},
                {"modelName", obj.modelName},
                {"position", obj.position},
                {"rotation", obj.rotation}
            };
        }

        static void from_json(const nlohmann::json& j, GameObjectCreateInfo& obj) {
            j.at("name").get_to(obj.name);
            j.at("modelName").get_to(obj.modelName);
            j.at("position").get_to(obj.position);
            j.at("rotation").get_to(obj.rotation);
        }
    };
}

namespace JSON {
    void SaveToFile(nlohmann::json& json, const std::string& filePath);
    bool LoadFile(nlohmann::json& json, const std::string filePath);

    void SaveLevel(const std::string& filePath, LevelCreateInfo& levelCreateInfo);
    LevelCreateInfo LoadLevel(const std::string& filePath);
}
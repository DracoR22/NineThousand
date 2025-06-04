#pragma once

#include <iostream>
#include <fstream> 
#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include "../Common/CreateInfo.hpp"

namespace nlohmann {
    template <>
    struct adl_serializer<glm::vec3> {
        static void to_json(nlohmann::json& j, const glm::vec3& value);
        static void from_json(const nlohmann::json& j, glm::vec3& value);
    };
    template <>
    struct adl_serializer<GameObjectCreateInfo> {
        static void to_json(nlohmann::json& j, const GameObjectCreateInfo& obj);
        static void from_json(const nlohmann::json& j, GameObjectCreateInfo& obj);
    };
}

namespace JSON {
    void SaveToFile(nlohmann::json& json, const std::string& filePath);
    bool LoadFile(nlohmann::json& json, const std::string filePath);

    void SaveLevel(const std::string& filePath, LevelCreateInfo& levelCreateInfo);
    LevelCreateInfo LoadLevel(const std::string& filePath);
}
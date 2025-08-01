#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

#include "../API/OpenGL/Shader.h"
#include "../API/OpenGL/Texture.h"

struct Character {
    unsigned int id;
    float x, y;
    float width, height;
    float xOffset, yOffset;
    float xAdvance;
};

namespace Text2D {
    inline std::unordered_map<unsigned int, Character> m_characters;

    void LoadFont(const std::string& path);
    float GetTextWidth(const std::string& text, float scale);
    float GetTextHeight(float scale);
}
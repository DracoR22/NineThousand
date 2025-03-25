#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

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
    void RenderFont(const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int shader);
}
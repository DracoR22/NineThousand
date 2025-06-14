#pragma once

#include <glm/glm.hpp>
#include "../Core/Window.h"
#include "../Common/CreateInfo.hpp"

class UIElement {
public:
    UIElement() = default;

    UIAlignment m_alignment = UIAlignment::TopLeft;
    bool m_useAligment = false;
    glm::vec2 m_aligmentOffset = glm::vec2(0.0f);

    bool m_isVisible = false;
    bool m_isText = false;

    float m_size = 1.0f;
    glm::vec2 m_position = glm::vec2(1.0f);

    glm::vec3 m_color = glm::vec3(1.0f);
    std::string m_textureName = "undefined";
    std::string m_text = "undefined";
public:
    glm::vec2 CalculatePosition();
    glm::mat4 GetModelMatrix();
};
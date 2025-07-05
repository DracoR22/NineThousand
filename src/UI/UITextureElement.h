#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Core/Window.h"

enum class UIAlignment {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Center,
    TopCenter,
    BottomCenter
};

class UITextureElement {
public:
    UITextureElement() = default;
    glm::vec2 CalculatePosition();
    glm::mat4 GetModelMatrix();
public:
    UIAlignment m_alignment = UIAlignment::TopLeft;
    glm::vec2 m_aligmentOffset = glm::vec2(0.0f);

    bool m_isVisible = false;

    float m_size = 1.0f;
    glm::vec2 m_position = glm::vec2(1.0f);

    glm::vec3 m_color = glm::vec3(1.0f);
    std::string m_textureName = "undefined";
};
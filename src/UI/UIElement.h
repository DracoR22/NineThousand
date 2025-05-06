#pragma once

#include <glm/glm.hpp>
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

struct UIElement {
    glm::vec2 m_size = glm::vec3(1.0f);
    glm::vec2 m_offset = glm::vec2(0.0f);
    glm::vec3 m_color = glm::vec3(1.0f);
    float m_scale = 1.0f;
    UIAlignment m_alignment = UIAlignment::TopLeft;

    glm::vec2 CalculatePosition();
    glm::mat4 GetModelMatrix();
};
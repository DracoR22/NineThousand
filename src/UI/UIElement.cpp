#include "UIElement.h"

glm::vec2 UIElement::CalculatePosition() {
    glm::vec2 pos;

    switch (m_alignment) {
    case UIAlignment::TopLeft:
        pos = { 0.0f, 0.0f };
        break;
    case UIAlignment::TopRight:
        pos = { Window::currentWidth - m_size.x, 0.0f };
        break;
    case UIAlignment::BottomLeft:
        pos = { 0.0f, Window::currentHeight - m_size.y };
        break;
    case UIAlignment::BottomRight:
        pos = { Window::currentWidth - m_size.x, Window::currentHeight - m_size.y };
        break;
    case UIAlignment::Center:
        pos = { (Window::currentWidth - m_size.x) * 0.5f, (Window::currentHeight - m_size.y) * 0.5f };
        break;
    case UIAlignment::TopCenter:
        pos = { (Window::currentWidth - m_size.x) * 0.5f, 0.0f };
        break;
    case UIAlignment::BottomCenter:
        pos = { (Window::currentWidth - m_size.x) * 0.5f, Window::currentHeight - m_size.y };
        break;
    }

    return pos + m_offset;
}

glm::mat4 UIElement::GetModelMatrix() {
    glm::vec2 position = CalculatePosition();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(m_size, 1.0f));
    return model;
}
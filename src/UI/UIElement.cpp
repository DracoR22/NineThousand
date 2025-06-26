#include "UIElement.h"

glm::vec2 UIElement::CalculatePosition() {
    switch (m_alignment) {
    case UIAlignment::TopLeft:
        m_position = { 0.0f, 0.0f };
        break;
    case UIAlignment::TopRight:
        m_position = { Window::m_windowWidth - m_size, 0.0f };
        break;
    case UIAlignment::BottomLeft:
        m_position = { 0.0f, Window::m_windowHeight - m_size };
        break;
    case UIAlignment::BottomRight:
        m_position = { Window::m_windowWidth - m_size, Window::m_windowHeight - m_size };
        break;
    case UIAlignment::Center:
        m_position = { (Window::m_windowWidth - m_size) * 0.5f, (Window::m_windowHeight - m_size) * 0.5f };
        break;
    case UIAlignment::TopCenter:
        m_position = { (Window::m_windowWidth - m_size) * 0.5f, 0.0f };
        break;
    case UIAlignment::BottomCenter:
        m_position = { (Window::m_windowWidth - m_size) * 0.5f, Window::m_windowHeight - m_size };
        break;
    }

    m_position += m_aligmentOffset;

    return m_position;
}

glm::mat4 UIElement::GetModelMatrix() {
    glm::mat4 model = glm::mat4(1.0f);

    if (m_useAligment) {
        CalculatePosition();
        model = glm::translate(model, glm::vec3(m_position, 0.0f));
    }
   
    model = glm::scale(model, glm::vec3(m_size, m_size, 1.0f));
    return model;
}
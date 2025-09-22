#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class BloodSplatterObject {
public:
    BloodSplatterObject() = default;
    BloodSplatterObject(glm::vec3 position, glm::vec3 forward);
    void Update(float deltaTime);

    const glm::mat4& GetModelMatrix() const { return m_modelMatrix; }
    const float GetLifeTime() const { return m_lifetime; }
    const int GetType() const { return m_type; }

private:
    int m_type = 9;
    float m_lifetime = 0.0f;
    glm::mat4 m_modelMatrix = glm::mat4(1.0f);
};
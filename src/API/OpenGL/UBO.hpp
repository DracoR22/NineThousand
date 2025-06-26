#pragma once

#include <glad/glad.h>
#include <vector>

class UBO {
public:
    UBO() = default;
    UBO(GLsizeiptr size, GLuint bindingIndex) :
        m_size(size), m_bindingIndex(bindingIndex) {
        Init();
    }

    void Init() {
        glGenBuffers(1, &m_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferData(GL_UNIFORM_BUFFER, m_size, nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template<typename T>
    void UpdateData(const T& data, GLintptr offset = 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T), &data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    template<typename T>
    void UpdateDataArray(const std::vector<T>& data, GLintptr offset = 0) {
        glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(T) * data.size(), data.data());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    GLuint GetID() const { return m_ubo; }

    void CleanUp() {
        if (m_ubo != 0) {
            glDeleteBuffers(1, &m_ubo);
            m_ubo = 0;
        }
    }
private:
    GLuint m_ubo = 0;
    GLsizeiptr m_size;
    GLuint m_bindingIndex;
};
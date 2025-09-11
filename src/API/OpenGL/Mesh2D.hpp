#pragma once 

#include "Shader.h"
#include "Texture.h"
#include "../../UI/Text2D.h"
#include "stb_image.h"

class Mesh2D {
private: 
	unsigned int m_VAO;
	unsigned int m_VBO;
    unsigned int m_textureID;
public: 
    Mesh2D() = default;

    Mesh2D(const float* vertices, size_t size) {
        Create(vertices, size);
    }

	void Create(const float* vertices, size_t size) {
        //float vertices[] = {
        //    // positions    // tex coords
        //     0.0f, 0.0f,     0.0f, 0.0f,
        //     1.0f, 0.0f,     1.0f, 0.0f,
        //     1.0f, 1.0f,     1.0f, 1.0f,

        //     0.0f, 0.0f,     0.0f, 0.0f,
        //     1.0f, 1.0f,     1.0f, 1.0f,
        //     0.0f, 1.0f,     0.0f, 1.0f
        //};

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
	}

    void RenderTexture(Shader& shader) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color, Shader& shader) {
        shader.activate();
        shader.setVec3("tintColor", color);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_VAO);

        float startX = x;

        std::vector<float> vertices;

        for (char c : text) {
            if (Text2D::m_characters.find(c) == Text2D::m_characters.end()) continue;

            const Character& ch = Text2D::m_characters[c];

            float texWidth = 512;
            float texHeight = 512;

            float xpos = startX + ch.xOffset * scale;
            float ypos = y + (ch.yOffset * scale);
            float w = ch.width * scale;
            float h = ch.height * scale;

            float vertices[6][4] = {
                // Bottom-left
                { xpos,     ypos + h, ch.x / texWidth,              (ch.y + ch.height) / texHeight },
                // Top-left
                { xpos,     ypos,     ch.x / texWidth,              ch.y / texHeight },
                // Top-right
                { xpos + w, ypos,     (ch.x + ch.width) / texWidth, ch.y / texHeight },

                // Bottom-left
                { xpos,     ypos + h, ch.x / texWidth,              (ch.y + ch.height) / texHeight },
                // Top-right
                { xpos + w, ypos,     (ch.x + ch.width) / texWidth, ch.y / texHeight },
                // Bottom-right
                { xpos + w, ypos + h, (ch.x + ch.width) / texWidth, (ch.y + ch.height) / texHeight }
            };

            glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            startX += (ch.xAdvance * scale);
        }

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    unsigned int GetVAO() {
        return m_VAO;
    }

    void Cleanup() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
};
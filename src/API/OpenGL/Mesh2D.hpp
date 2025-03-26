#pragma once 

#include "Shader.h"
#include "Texture.h"
#include "../../UI/Text2D.h"
#include "stb_image.h"

class Mesh2D {
private: 
	unsigned int m_VAO;
	unsigned int m_VBO;
    unsigned int m_fontTextureID;

    void LoadTexture(const std::string& path) {
        int width, height, nrChannels;

        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Failed to load font texture: " << path << std::endl;
            return;
        }

        glGenTextures(1, &m_fontTextureID);
        glBindTexture(GL_TEXTURE_2D, m_fontTextureID);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RED;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Texture parameters for text rendering
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }

public: 
	void Create() {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        LoadTexture("resources/fonts/sans.png");
	}

    unsigned int GetVAO() {
        return m_VAO;
    }

    void RenderUI(const std::string& text, float x, float y, float scale, glm::vec3 color, Shader& shader) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_fontTextureID);

        shader.activate();
        shader.setVec3("textColor", color);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_VAO);

        // Iterate through each character in the string
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

    void Cleanup() {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
};
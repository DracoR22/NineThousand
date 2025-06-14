#include "Text2D.h"

namespace Text2D {
    float m_maxCharHeight = 0.0f;

    void LoadFont(const std::string& path) {
        std::ifstream file(path);

        if (!file.is_open()) {
            std::cerr << "Failed to open font file: " << path << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            iss >> key;

            if (key == "char") {
                Character ch = {};
                std::string pair;

                while (iss >> pair) {
                    auto delimiterPos = pair.find('=');
                    std::string name = pair.substr(0, delimiterPos);
                    std::string value = pair.substr(delimiterPos + 1);

                    if (name == "id") ch.id = std::stoi(value);
                    else if (name == "x") ch.x = std::stof(value);
                    else if (name == "y") ch.y = std::stof(value);
                    else if (name == "width") ch.width = std::stof(value);
                    else if (name == "height") ch.height = std::stof(value);
                    else if (name == "xoffset") ch.xOffset = std::stof(value);
                    else if (name == "yoffset") ch.yOffset = std::stof(value);
                    else if (name == "xadvance") ch.xAdvance = std::stof(value);
                }

                if (ch.height > m_maxCharHeight) {
                    m_maxCharHeight = ch.height;
                }

                m_characters[ch.id] = ch;
            }

            // Extract texture information (page section)
            if (key == "page") {
                std::string pair;
                while (iss >> pair) {
                    if (pair.find("file=") == 0) {
                        std::string texturePath = pair.substr(5);
                        texturePath.erase(remove(texturePath.begin(), texturePath.end(), '"'), texturePath.end());
                        std::cout << "Font texture: " << texturePath << std::endl;
                    }
                }
            }
        }

        std::cout << "Loaded " << m_characters.size() << " characters.\n";
    }

    float GetTextWidth(const std::string& text, float scale) {
        float width = 0.0f;
        for (char c : text) {
            if (m_characters.find(c) == m_characters.end()) continue;
            const Character& ch = m_characters[c];
            width += ch.xAdvance * scale;
        }
        return width;
    }

    float GetTextHeight(float scale) {
        return m_maxCharHeight * scale;
    }
}
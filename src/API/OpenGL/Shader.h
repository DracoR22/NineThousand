#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_set>

class Shader {
public:
	Shader() = default;

	void generate(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath);

	void load(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr);

	void activate();

	std::vector<Shader*> shaders;

	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);
	std::string LoadFile(const std::string& filepath);
	std::string PreprocessShader(const std::string& filepath, std::unordered_set<std::string>& includedFiles);


	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);

	void setFloat(const std::string& name, float value);
	void set3Float(const std::string& name, float v1, float v2, float v3);
	void set3Float(const std::string& name, glm::vec3 v);
	void set4Float(const std::string& name, float v1, float v2, float v3, float v4);
	void set4Float(const std::string& name, glm::vec4 v);

	void setMat3(const std::string& name, glm::mat3 val);
	void setMat4(const std::string& name, glm::mat4 val);

	void setVec3(const std::string& name, float x, float y, float z);
	void setVec3(const std::string& name, glm::vec3 value);

	void setVec2(const std::string& name, float x, float y);
	void setVec2(const std::string& name, glm::vec2 value);
public:
	unsigned int id;
};
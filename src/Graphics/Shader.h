#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
	unsigned int id;

	Shader();

	void generate(const char* vertexShaderPath, const char* fragmentShaderPath);

	void load(const char* vertexShaderPath, const char* fragmentShaderPath);

	/*static void hotLoadShaders(std::vector<Shader*>& shaders);*/

	void activate();

	std::vector<Shader*> shaders;

	// utility functions 
	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	// uniform functions
	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void set3Float(const std::string& name, float v1, float v2, float v3);
	void set3Float(const std::string& name, glm::vec3 v);
	void set4Float(const std::string& name, float v1, float v2, float v3, float v4);

	//void set4Float(const std::string& name, aiColor4D color);
	void set4Float(const std::string& name, glm::vec4 v);
	void setMat4(const std::string& name, glm::mat4 val);

	void setVec3(const std::string& name, float x, float y, float z);
	void setVec3(const std::string& name, glm::vec3 value);
};
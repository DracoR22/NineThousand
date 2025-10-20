#include "Shader.h"
#include <regex>

void Shader::load(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath) {
	if (id != 0) {
		glDeleteProgram(id);  
	}

	generate(vertexShaderPath, fragmentShaderPath, geometryShaderPath);
}

void Shader::generate(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath) {
	int success;
	char infolog[512];

	const char* debugShaderPathPrefix = "../../../resources/shaders/";
	const char* releaseShaderPathPrefix = "resources/shaders/";

	std::string vertexShaderFile = std::string(debugShaderPathPrefix) + vertexShaderPath;
	std::string fragmentShaderFile = std::string(debugShaderPathPrefix) + fragmentShaderPath;

	GLuint vertexShader = compileShader(vertexShaderFile.c_str(), GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragmentShaderFile.c_str(), GL_FRAGMENT_SHADER);

	GLuint geometryShader = 0;

	if (geometryShaderPath != nullptr) {
		std::string geometryShaderFile = std::string(debugShaderPathPrefix) + geometryShaderPath;
		geometryShader = compileShader(geometryShaderFile.c_str(), GL_GEOMETRY_SHADER);
	}

	id = glCreateProgram();

	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	if (geometryShaderPath != nullptr) {
		glAttachShader(id, geometryShader);
	}

	glLinkProgram(id);

	// catch program errors
	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infolog);
		std::cout << "Linking error: " << std::endl << infolog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
	if (geometryShaderPath != nullptr) {
		glDeleteShader(geometryShader);
	}
}

void Shader::activate() {
	glUseProgram(id);
}

std::string Shader::loadShaderSrc(const char* filepath) {
	/*std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filename);

	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
	}
	else {
		std::cout << "Could not open Shader " << filename << std::endl;
	}

	file.close();

	return ret;*/
	std::unordered_set<std::string> includedFiles;
	return PreprocessShader(filepath, includedFiles);
}

GLuint Shader::compileShader(const char* filepath, GLenum type) {
	int success;
	char infolog[512];

	GLuint ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);

	// catch errors
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(ret, 512, NULL, infolog);
		std::cout << "Error with fragment shader com: " << std::endl << infolog << std::endl;
	}

	return ret;
}

std::string Shader::LoadFile(const std::string& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cout << "Cannot open shader file: " + filepath << std::endl;
		throw std::runtime_error("Cannot open shader file: " + filepath);
	}
	std::stringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

std::string Shader::PreprocessShader(const std::string& filepath, std::unordered_set<std::string>& includedFiles) {
	if (includedFiles.count(filepath)) {
		return "";
	}
	includedFiles.insert(filepath);

	std::string src = LoadFile(filepath);
	std::stringstream processed;
	std::regex includeRegex("^\\s*#include\\s+\"([^\"]+)\"\\s*");

	std::istringstream stream(src);
	std::string line;
	std::string baseDir = filepath.substr(0, filepath.find_last_of("/\\") + 1);

	while (std::getline(stream, line)) {
		std::smatch match;
		if (std::regex_match(line, match, includeRegex)) {
			std::string includePath = baseDir + match[1].str();
			processed << PreprocessShader(includePath, includedFiles) << "\n";
		}
		else {
			processed << line << "\n";
		}
	}

	return processed.str();
}

void Shader::setBool(const std::string& name, bool value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::set3Float(const std::string& name, float v1, float v2, float v3) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::set3Float(const std::string& name, glm::vec3 v) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::set4Float(const std::string& name, float v1, float v2, float v3, float v4) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::set4Float(const std::string& name, glm::vec4 v) {
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::setMat3(const std::string& name, glm::mat3 val) {
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setMat4(const std::string& name, glm::mat4 val) {
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) {
	glUniform3f(glGetUniformLocation(id, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec2(const std::string& name, float x, float y) {
	glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVec2(const std::string& name, glm::vec2 value) {
	glUniform2f(glGetUniformLocation(id, name.c_str()), value.x, value.y);
}

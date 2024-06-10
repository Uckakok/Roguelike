#include"pch.h"
#include"Shader.h"
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include"Renderer.h"


// Pobiera lokalizacjê (indeks) jednostki uniformu o podanej nazwie
int Shader::getUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];
	GLCall(int location = glGetUniformLocation(m_RendererID, name.c_str()));
	if (location == -1)
		MessageBox(NULL, L"Shader doesn't exist!", L"Error", MB_OK | MB_ICONERROR);

	m_UniformLocationCache[name] = location;
	return location;
}

// Parsuje plik shadera i tworzy program shaderowy
Shader::Shader(const std::string& filepath) : m_FilePath(filepath), m_RendererID(0)
{
	shaderProgramSource source = parseShader(filepath);
	m_RendererID = createShader(source.vertexSource, source.fragmentSource);
}

Shader::~Shader()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Shader::bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Shader::unbind()
{
	GLCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &matrix[0][0]));
}

// Tworzy program shaderowy na podstawie Ÿróde³ shaderów
unsigned int Shader::createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;

}

// Kompiluje shader o podanym typie i Ÿródle
unsigned int Shader::compileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		MessageBox(NULL, L"Failed to compile the shader", L"Error", MB_OK | MB_ICONERROR);
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;

	}

	return id;
}

shaderProgramSource Shader::parseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class shaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	shaderType type = shaderType::NONE;
	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = shaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = shaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}
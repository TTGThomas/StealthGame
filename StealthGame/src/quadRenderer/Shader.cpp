#include "Shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	Init(vertexShaderPath, fragmentShaderPath);
}

Shader::~Shader()
{
}

void Shader::Cleanup()
{
	glDeleteProgram(m_program);
}

void Shader::Init(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	unsigned int vertex = CompileShader(vertexShaderPath, GL_VERTEX_SHADER);
	unsigned int fragment = CompileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertex);
	glAttachShader(m_program, fragment);
	glLinkProgram(m_program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Bind()
{
	glUseProgram(m_program);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

unsigned int Shader::CompileShader(const char* path, GLenum type)
{
	std::ifstream file;
	file.open(path);

	if (!file.is_open())
		std::cout << "fail to load file in path: " << path << std::endl;

	std::string source = "";
	std::string line = "";
	while (std::getline(file, line))
	{
		source.append(line);
		source.append("\n");
	}

	unsigned int shader = glCreateShader(type);
	const char* shaderSource = source.c_str();
	glShaderSource(shader, 1, &shaderSource, nullptr);
	glCompileShader(shader);

	int good = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &good);
	if (!good)
	{
		char infoLog[512] = {};
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "error with " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " :\n" << infoLog << std::endl;
	}
	return shader;
}

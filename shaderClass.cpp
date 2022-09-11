#include "shaderClass.h"

std::string get_file_contents(const char* fileName)
{
	std::ifstream in(fileName, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw (errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// We will be able to find our vertex shader by the ID stored in it (by reference)
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// (ID of the shader, how many strings are we using for THIS SPECIFIC shader, where to read from, DEFAULT)
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compiles the text it read as a usable shadercode (ID of the shader)
	glCompileShader(vertexShader);

	// We will be able to find our fragment shader by the ID stored in it (by reference)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// (ID of the shader, how many strings are we using for THIS SPECIFIC shader, where to read from, DEFAULT)
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compiles the text it read as a usable shadercode (ID of the shader)
	glCompileShader(fragmentShader);

	// Creates a shader program
	// The computer will run this and we can store our shaders in it
	ID = glCreateProgram();
	// We attach our shaders to the shaderprogram so it will run on the GPU
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	// Link all the shaders together into the shader program
	// Create a rendering pipeline for the shader program: FIRST: VERTEX, SECOND: FRAGMENT, etc...
	glLinkProgram(ID);

	// The compiled shaders are already stored in the shaderprogram so we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
	glUseProgram(ID);
}

void Shader::Delete()
{
	glDeleteProgram(ID);
}

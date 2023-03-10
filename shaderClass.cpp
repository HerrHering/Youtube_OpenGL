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

void Shader::CompileErrors(unsigned int shader, const char* type)
{
	GLint hasCompiled;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
		}
	}
	else
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << std::endl;
		}
	}
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
	CompileErrors(vertexShader, "VERTEX");

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Geometry Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Fragment Shader into machine code
	glCompileShader(fragmentShader);
	// Check if Fragment Shader compiled succesfully
	CompileErrors(fragmentShader, "FRAGMENT");

	// Creates a shader program
	// The computer will run this and we can store our shaders in it
	ID = glCreateProgram();
	// We attach our shaders to the shaderprogram so it will run on the GPU
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);

	// Link all the shaders together into the shader program
	// Create a rendering pipeline for the shader program: FIRST: VERTEX, SECOND: FRAGMENT, etc...
	glLinkProgram(ID);
	CompileErrors(ID, "PROGRAM");

	// The compiled shaders are already stored in the shaderprogram so we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const char* vertexFile, const char* geometryFile, const char* fragmentFile)
{
	std::string vertexCode = get_file_contents(vertexFile);
	std::string geometryCode = get_file_contents(geometryFile);
	std::string fragmentCode = get_file_contents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* geometrySource = geometryCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// We will be able to find our vertex shader by the ID stored in it (by reference)
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	// (ID of the shader, how many strings are we using for THIS SPECIFIC shader, where to read from, DEFAULT)
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	// Compiles the text it read as a usable shadercode (ID of the shader)
	glCompileShader(vertexShader);
	CompileErrors(vertexShader, "VERTEX");

	// Create Geometry Shader Object and get its reference
	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	// Attach Geometry Shader source to the Geometry Shader Object
	glShaderSource(geometryShader, 1, &geometrySource, NULL);
	// Compile the Geometry Shader into machine code
	glCompileShader(geometryShader);
	// Check if Geometry Shader compiled succesfully
	CompileErrors(geometryShader, "GEOMETRY");

	// Create Fragment Shader Object and get its reference
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	// Attach Fragment Shader source to the Geometry Shader Object
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	// Compile the Fragment Shader into machine code
	glCompileShader(fragmentShader);
	// Check if Fragment Shader compiled succesfully
	CompileErrors(fragmentShader, "FRAGMENT");

	// Creates a shader program
	// The computer will run this and we can store our shaders in it
	ID = glCreateProgram();
	// We attach our shaders to the shaderprogram so it will run on the GPU
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, geometryShader);
	glAttachShader(ID, fragmentShader);

	// Link all the shaders together into the shader program
	// Create a rendering pipeline for the shader program: FIRST: VERTEX, SECOND: FRAGMENT, etc...
	glLinkProgram(ID);
	CompileErrors(ID, "PROGRAM");

	// The compiled shaders are already stored in the shaderprogram so we can delete them
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
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

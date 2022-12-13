#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

std::string get_file_contents(const char* fileName);

/// <summary>
/// Vertex -> Geometry (optional) -> Fragment
/// </summary>
class Shader
{
private:
	void CompileErrors(unsigned int shader, const char* type);
public:
	GLuint ID;
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader(const char* vertexFile, const char* geometryFile, const char* fragmentFile);

	void Activate();
	void Delete();
};

#endif
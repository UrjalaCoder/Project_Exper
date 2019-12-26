#include "Shader.h"

Shader::Shader(const char *vertex_path, const char *fragment_path) {
	std::string vertex_string;
	std::string fragment_string;

	std::ifstream vertex_file;
	std::ifstream fragment_file;

	vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		vertex_file.open(vertex_path);
		fragment_file.open(fragment_path);

		std::stringstream vertex_stream, fragment_stream;

		vertex_stream << vertex_file.rdbuf();
		fragment_stream << fragment_file.rdbuf();

		vertex_file.close();
		fragment_file.close();

		vertex_string = vertex_stream.str();
		fragment_string = fragment_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_READ" << std::endl;
	}

	// Set the const char variables.
	const char *vertex_code = vertex_string.c_str();
	const char *fragment_code = fragment_string.c_str();

	GLuint vertex, fragment;

	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertex_code, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	success = 1;
	infoLog[512] = { 0 };

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragment_code, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	success = 1;
	infoLog[512] = { 0 };

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::use() {
	glUseProgram(ID);
}

void Shader::set_bool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set_int(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

#ifndef SHADER
#define SHADER

#ifndef GL
#define GL
#include <SDL2/SDL.h>
#include <GL/glew.h>
#endif

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
public:
	GLuint ID;
	Shader(const GLchar *vertex_path, const GLchar *fragment_path);
	void use();

	void set_bool(const std::string &name, bool value) const;
	void set_int(const std::string &name, int value) const;
	void set_float(const std::string &name, float value) const;
};

#endif

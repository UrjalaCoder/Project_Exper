#ifndef RENDERER
#define RENDERER

#ifndef GL
#define GL
#include <SDL2/SDL.h>
#include <GL/glew.h>
#endif

#include "../Shader/Shader.h"

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


// Simple 3D Vertex.
struct Vertex3D {
	float x, y, z;
};

// 3D Vertex with color data.
struct ColouredVertex3D {
	float x, y, z, r, g, b;
};

// 3D Vertex with texture data.
struct TexturedVertex3D {
	float x, y, z, t, s;
};

class Renderer {
private:
	unsigned int WINDOW_WIDTH, WINDOW_HEIGHT;
	SDL_GLContext render_context;
	SDL_Window *render_window;
	SDL_Event event;
	bool error = false;
	bool running = true;
	bool wireframe = false;
	bool debug_mode = false;
	std::string load_shader(const char *filename);
	void Show_Error(std::string error_message);
	void setTexture(GLuint &texture, const char* filename, bool isAlpha, bool flipped);
	Shader *shader;

public:
    Renderer(int width, int height);
    void render();
};


#endif

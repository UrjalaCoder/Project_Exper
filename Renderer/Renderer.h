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


struct Vertex3D {
	float x, y, z;
};

struct ColouredVertex3D {
	float x, y, z, r, g, b;
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
	Shader *shader;

public:
    Renderer(int width, int height);
    void render();
};


#endif

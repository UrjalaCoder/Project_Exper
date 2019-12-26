
#ifndef RENDERER

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <ctime>

#define RENDERER

struct Vertex3D {
	float x, y, z;
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
	void render_trig(std::vector<Vertex3D> vertices, float t);

	std::string vertex_shader;
	std::string fragment_shader;
	void render(float t);

public:
    Renderer(int width, int height);
    void start();
};


#endif

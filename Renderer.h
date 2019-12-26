
#ifndef RENDERER

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <vector>

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
	void Show_Error(std::string error_message);
	void render_trig(std::vector<Vertex3D> vertices);

	const char *vertexShader =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }";

	const char *fragmentShader =
    "#version 330 core\n"
    "out vec4 col;\n"
    "void main() {\n"
    "col = vec4(1.0, 1.0, 1.0, 1.0); }";

public:
    Renderer(int width, int height);
    void start();
    void render();
};


#endif

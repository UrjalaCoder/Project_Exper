
#ifndef RENDERER
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#define RENDERER
class Renderer {
private:
  unsigned int WINDOW_WIDTH, WINDOW_HEIGHT;
  SDL_GLContext render_context;
  SDL_Window *render_window;
  SDL_Event event;
  bool error = false;
  bool running = true;
  void Show_Error(std::string error_message);
public:
  Renderer(int width, int height);
  void start();
  void render();
};

#endif

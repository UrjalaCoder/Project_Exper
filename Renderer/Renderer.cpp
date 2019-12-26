#include "Renderer.h"

Renderer::Renderer(int window_width, int window_height) {
    WINDOW_WIDTH = window_width;
    WINDOW_HEIGHT = window_height;


    /* Set OpenGL versions [1, 3] */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    const std::string title = "Project Exper";

    /* Try to initialize SDL2 */
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error = true;
        Show_Error("Failed to init!");
    } else {
        /* Initializing window */
        render_window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
        );
        if(render_window == nullptr) {
            error = true;
            Show_Error("Could not create window!");
        } else {
            /* Initializing render context. */
            render_context = SDL_GL_CreateContext(render_window);
            if(render_context == nullptr) {
                error = true;
                Show_Error("Could not create render context!");
            } else {
                /* Init GLEW */
                glewInit();
                // Initialize the OpenGL viewport.
                glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
            }
        }
    }

    /* Done initialization! */
	/* Finally load shaders that we need. */
	vertex_shader = load_shader("vertex_shader");
	fragment_shader = load_shader("fragment_shader");
}

void Renderer::start() {
    if(error) {
        Show_Error("Unknown error!");
        return;
    }
	float time = 0.001f;
    while(running) {
        while(SDL_PollEvent(&event) != 0) {
            if(event.type == SDL_QUIT) {
                running = false;
            } else if(event.type == SDL_KEYDOWN) {
				SDL_Keycode w = event.key.keysym.sym;
				if(w == 119) {
					wireframe = !wireframe;
				}
			}
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* MAIN RENDER HERE! */
        render(time);

        SDL_GL_SwapWindow(render_window);

		time += 0.001f;
    }

    SDL_GL_DeleteContext(render_context);
    SDL_DestroyWindow(render_window);
    render_window = nullptr;
    SDL_Quit();
    return;
}

void Renderer::render(float time) {
    std::vector<ColouredVertex3D> vertices = {
		{-0.5f, 0.0f, 0.0f, 	1.0f, 0.0f, 0.0f},
		{0.5f, 0.0f, 0.0f, 		0.0f, 1.0f, 0.0f},
		{0.0f, 0.5f, 0.0f, 		0.0f, 0.0f, 1.0f},
    };
    render_trig(vertices, time);
}

void Renderer::render_trig(std::vector<ColouredVertex3D> vertex_vector, float time) {
    GLfloat vertices[vertex_vector.size() * 6] = { 0.0f };

    for(int i = 0; i < vertex_vector.size(); ++i) {
        int vertex_index = i * 6;
        vertices[vertex_index] = vertex_vector[i].x;
        vertices[vertex_index + 1] = vertex_vector[i].y;
        vertices[vertex_index + 2] = vertex_vector[i].z;

		// Colours
		vertices[vertex_index + 3] = vertex_vector[i].r;
		vertices[vertex_index + 4] = vertex_vector[i].g;
		vertices[vertex_index + 5] = vertex_vector[i].b;
    }


	const char *vertex_string = vertex_shader.c_str();
	const char *fragment_string = fragment_shader.c_str();

    /* Create vertex shaders */
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderID, 1, &vertex_string, nullptr);
    glCompileShader(vertexShaderID);

    /* Fragment shaders */
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderID, 1, &fragment_string, nullptr);
    glCompileShader(fragmentShaderID);

	/* DEBUG Code! */
	if(debug_mode) {
		int success;
		char infoLog[512];
		glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);

		if(!success)
		{
			glGetShaderInfoLog(vertexShaderID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		success = 1;
		glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
		if(!success) {
			glGetShaderInfoLog(fragmentShaderID, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
	}
	/* END DEBUG */

    /* Apply vertex and fragment shaders */
    GLuint shaderProgramID = glCreateProgram();


    glAttachShader(shaderProgramID, vertexShaderID);
    glAttachShader(shaderProgramID, fragmentShaderID);
    glLinkProgram(shaderProgramID);

    /* Detach and delete shaders the we don't need anymore. */
    glDetachShader(shaderProgramID, vertexShaderID);
    glDetachShader(shaderProgramID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    /* Create and initialize vertex buffer */
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* Apply colour */
	float green_value = (sin(time) / 2.0f) + 0.5f;

	int vertex_colour_location = glGetUniformLocation(shaderProgramID, "colour");
	// glUseProgram(shaderProgramID);

    /* Use the combined shader */
    glUseProgram(shaderProgramID);
	glUniform4f(vertex_colour_location, 0.0f, green_value, 0.0f, 1.0f);

	// Position attributes.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

	// Colour attributes.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


	/* Bind the GL array buffer to vertex buffer */
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    /* Draw! */
	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	glDrawArrays(GL_TRIANGLES, 0, vertex_vector.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glUseProgram(NULL);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    return;
}

void Renderer::Show_Error(std::string error_message) {
    std::string msg = "Window could not be created: ";
    msg.append(SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
}

std::string Renderer::load_shader(const char *filename) {

	std::string extension(".glsl");
	std::string full_path = std::string("shaders/") + std::string(filename) + extension;

	std::ifstream in(full_path);
	std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	return str;
}

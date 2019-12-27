#include "Renderer.h"
#include "../TextureLoader/TextureLoader.cpp"

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
	shader = new Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");
}

void Renderer::render() {
    if(error) {
        Show_Error("Unknown error!");
        return;
    }

	std::vector<TexturedVertex3D> vertex_vector = {
		/* Coordinates				Colors			Texture Pos */
		{0.5f, 0.5f, 0.0f, 		1.0f, 0.0f, 0.0f, 	1.0f, 1.0f},
		{0.5f, -0.5f, 0.0f, 	0.0f, 1.0f, 0.0f, 	1.0f, 0.0f},
		{-0.5f, -0.5f, 0.0f, 	0.0f, 0.0f, 1.0f, 	0.0f, 0.0f},
		{-0.5f, 0.5f, 0.0f, 	1.0f, 1.0f, 0.0f, 	0.0f, 1.0f}
    };

	// Vertices setup
	GLfloat vertices[vertex_vector.size() * 8] = { 0.0f };

	// Texture handling.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture
	int width, height, nr_channels;
	unsigned char* data = stbi_load("container.jpg", &width, &height, &nr_channels, 0);
	if(data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture!" << std::endl;
	}
	stbi_image_free(data);

    for(int i = 0; i < vertex_vector.size(); ++i) {
        int vertex_index = i * 8;
        vertices[vertex_index] = vertex_vector[i].x;
        vertices[vertex_index + 1] = vertex_vector[i].y;
        vertices[vertex_index + 2] = vertex_vector[i].z;

		// Colours
		vertices[vertex_index + 3] = vertex_vector[i].r;
		vertices[vertex_index + 4] = vertex_vector[i].g;
		vertices[vertex_index + 5] = vertex_vector[i].b;

		vertices[vertex_index + 6] = vertex_vector[i].t;
		vertices[vertex_index + 7] = vertex_vector[i].s;
    }

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

    /* Create and initialize vertex buffer */
    GLuint vertexBufferID, vertexArrayID;
	GLuint EBO;

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attributes.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// Colour attributes.
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	// Texture
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


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

		// Texture bind.
		glBindTexture(GL_TEXTURE_2D, texture);
        /* MAIN RENDER HERE! */
		shader->use();
		glBindVertexArray(vertexArrayID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        SDL_GL_SwapWindow(render_window);
    }

    glUseProgram(NULL);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    SDL_GL_DeleteContext(render_context);
    SDL_DestroyWindow(render_window);
    render_window = nullptr;
    SDL_Quit();
    return;
}

void Renderer::Show_Error(std::string error_message) {
    std::string msg = "Window could not be created: ";
    msg.append(SDL_GetError());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Init Failed", msg.c_str(), nullptr);
}

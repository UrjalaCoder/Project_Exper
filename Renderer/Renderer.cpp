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

void Renderer::setTexture(GLuint &texture, const char *filename, bool isAlpha, bool flipped) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture
	int width, height, nr_channels;
	stbi_set_flip_vertically_on_load(flipped);
	unsigned char* data = stbi_load(filename, &width, &height, &nr_channels, 0);
	if(data) {
		GLuint format = isAlpha ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture!" << std::endl;
	}

	stbi_image_free(data);
}

void Renderer::handle_input() {
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
}

void Renderer::render() {
    if(error) {
        Show_Error("Unknown error!");
        return;
    }

	int vertex_count = 4;
	GLfloat vertices[vertex_count * 5] = {
		/* Coordinates			Texture Pos */
		0.5f, 0.5f, 0.0f,  		1.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 		1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f, 	0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f, 		0.0f, 1.0f
    };

	GLuint indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// Texture handling.
	GLuint texture1;
	GLuint texture2;
	setTexture(texture1, "container.jpg", false, false);
	setTexture(texture2, "awesomeface.png", true, true);

	/* Set textures to fragment shader uniforms */
	shader->use();
	shader->set_int("texture1", 0);
	shader->set_int("texture2", 1);


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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// Texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	float time = 0.0f;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    while(running) {
        handle_input();

		// Texture bind.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

        /* MAIN RENDER HERE! */
		shader->use();
		/* Transformation */
		/* Rotation and scaling */
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.75, -0.75, 0.0));
		trans = glm::rotate(trans, time / 2000.0f, glm::vec3(0.0, 0.0, 1.0));
		trans = glm::scale(trans, glm::vec3(1.0));

		GLuint transformLoc = glGetUniformLocation(shader->ID, "transform");
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

		shader->set_float("alpha", 0.2f);

		glBindVertexArray(vertexArrayID);

		if(wireframe) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

        glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        SDL_GL_SwapWindow(render_window);
		time += 1.0f;
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

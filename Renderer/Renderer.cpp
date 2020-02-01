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

void Renderer::setTexture(GLuint &texture, std::string filename, bool isAlpha, bool flipped) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture
	int width, height, nr_channels;
	stbi_set_flip_vertically_on_load(flipped);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nr_channels, 0);
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

std::vector<Vertex3D> generateTerrain(double zOffset, TerrainGenerator gen)
{
	std::vector<Vertex3D> vertexVector;
	std::vector<std::vector<TerrainQuad>> terrain = gen.Generate(100, 100, 1.0, zOffset);
	std::array<int, 6> cornerIndices = { { 0, 1, 2, 0, 3, 2 } };
	double size = terrain[0][0].size;
	for(int row = 0; row < terrain.size() - 1; row++)
	{
		for(int i = 0; i < terrain[row].size(); i++)
		{
			std::array<std::array<double, 3>, 4> corners = {{
				{ terrain[row][i].x, terrain[row][i].y, terrain[row][i].elevations[0] },
				{ terrain[row][i].x, terrain[row + 1][i].y, terrain[row + 1][i].elevations[0] },
				{ terrain[row][i].x + size, terrain[row + 1][i].y, terrain[row + 1][i].elevations[3] },
				{ terrain[row][i].x + size, terrain[row][i].y, terrain[row][i].elevations[3] }
			}};
			for(int vertexIndex = 0; vertexIndex < cornerIndices.size(); vertexIndex++)
			{
				std::array<double, 3> corner = corners[cornerIndices[vertexIndex]];
				// std::cout << "Corner: " << corner[0] << ", " << corner[1] << ", " << corner[2] << std::endl;
				vertexVector.push_back({ corner[0], corner[1], corner[2] });
			}
		}
	}

	return vertexVector;
}

void Renderer::render(TerrainGenerator gen) {
    if(error) {
        Show_Error("Unknown error!");
        return;
    }

	double z = 0.0f;
	std::vector<Vertex3D> vertexVector = generateTerrain(z, gen);

	std::cout << vertexVector[0].x << std::endl;

	GLfloat vertices[vertexVector.size() * 3];
	for(int i = 0; i < vertexVector.size(); i++)
	{
		int glIndex = i * 3;
		// std::cout << "X: " << vertexVector[i].x << " Y: " << vertexVector[i].y << " Z: " << vertexVector[i].z << std::endl;
		vertices[glIndex] = vertexVector[i].x;
		vertices[glIndex+1] = vertexVector[i].y;
		vertices[glIndex+2] = vertexVector[i].z;
	}

	/* Set textures to fragment shader uniforms */
	shader->use();

    /* Create and initialize vertex buffer */
    GLuint vertexBufferID, vertexArrayID;
	GLuint EBO;

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attributes.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	unsigned long long int time = 1;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);




	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(-50.0f, -5.0f, z * 100));




	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::translate(model, glm::vec3(0, z * 100, 0));

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

	int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


	int viewLoc = glGetUniformLocation(shader->ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


	int projectionLoc = glGetUniformLocation(shader->ID, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glEnable(GL_DEPTH_TEST);
	// running = false;
    while(running) {
        handle_input();
        /* MAIN RENDER HERE! */
		shader->use();
		/* Transformation */
		/* Rotation and scaling */

		std::vector<Vertex3D> vertexVector = generateTerrain(z, gen);
		for(int i = 0; i < vertexVector.size(); i++)
		{
			int glIndex = i * 3;
			// std::cout << "X: " << vertexVector[i].x << " Y: " << vertexVector[i].y << " Z: " << vertexVector[i].z << std::endl;
			vertices[glIndex] = vertexVector[i].x;
			vertices[glIndex+1] = vertexVector[i].y;
			vertices[glIndex+2] = vertexVector[i].z;
		}


	    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


		glBindVertexArray(vertexArrayID);

		if(wireframe) {
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
		}

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, vertexVector.size());
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        SDL_GL_SwapWindow(render_window);
		time += 1;
		z += 0.0001;
    }

    glUseProgram(NULL);

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

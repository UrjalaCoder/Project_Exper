OBJS = main.cpp ./Renderer/Renderer.cpp ./Shader/Shader.cpp ./TextureLoader/TextureLoader.cpp ./TerrainGenerator/PerlinNoise.cpp ./TerrainGenerator/TerrainGenerator.cpp
LINK_OBJS = main.o Renderer.o Shader.o PerlinNoise.o TerrainGenerator.o
LINKER_OPTIONS =  -lSDL2 -lGLEW -lGLU -lGL
OBJ_NAME = exper

# This is the target that compiles our executable
all: $(OBJS)
	@echo "Building"
	g++ -c -w -std=c++14 $(OBJS) -I.
	g++ -w $(LINK_OBJS) $(LINKER_OPTIONS) -o $(OBJ_NAME)
	@echo "Cleaning build files"
	rm -f $(LINK_OBJS)
	rm -f TextureLoader.o
	mv $(OBJ_NAME) build/$(OBJ_NAME)

OBJS = main.cpp ./Renderer/Renderer.cpp
LINK_OBJS = main.o Renderer.o
LINKER_OPTIONS = -lSDL2 -lGLEW -lGLU -lGL
OBJ_NAME = exper

# This is the target that compiles our executable
all: $(OBJS)
	@echo "Building"
	g++ -c -w $(OBJS)
	g++ -w $(LINK_OBJS) $(LINKER_OPTIONS) -o $(OBJ_NAME)
	@echo "Cleaning build files"
	rm -f $(LINK_OBJS)
	mv $(OBJ_NAME) build/$(OBJ_NAME)

LIBS=-lsfml-graphics -lsfml-window -lsfml-system
GCC_OPTIONS=-std=c++14

all:
	@echo "Building"
	g++ $(GCC_OPTIONS) -c "main.cpp" -o main.o
	g++ -o exper main.o $(LIBS) 

#include "main.h"
#include "TerrainGenerator/TerrainGenerator.h"

int main()
{
	TerrainGenerator gen;

	// for(int y = 0; y < 10; y++)
	// {
	// 	for(int x = 0; x < 10; x++)
	// 	{
	// 		std::cout << "Xindex: " << x << " yindex: " << y << std::endl;
	// 		std::cout << "X: " << terrain[y][x].x << " Y: " << terrain[y][x].y << std::endl;
	// 	}
	// }

    Renderer renderer(640, 480);
    renderer.render(gen);
    return 0;
}

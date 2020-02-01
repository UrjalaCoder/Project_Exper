#include "TerrainGenerator.h"

const unsigned int NOISE_SEED = 1337;

TerrainGenerator::TerrainGenerator()
{
	nn = PerlinNoise(NOISE_SEED);
}

std::vector<std::vector<double>> TerrainGenerator::generate_plane(int width, int height, double z)
{
	std::vector<std::vector<double>> result;
	double coersionFactor = 0.2f;
	double yOff = 0.0f;
	for(int y = 0; y < height; y++)
	{
		std::vector<double> row;
		double xOff = 0.0f;
		for(int x = 0; x < width; x++)
		{
			row.push_back(nn.noise(xOff, yOff, z));
			xOff += coersionFactor;
		}
		yOff += coersionFactor;
		result.push_back(row);
	}

	// Generate the quads.

	return result;
}

std::vector<std::vector<TerrainQuad>> TerrainGenerator::Generate(int width, int height, double quadSize, double zOffset)
{
	double power = 0.6;
	double yOff = zOffset;
	int y = 0;
	std::vector<std::vector<TerrainQuad>> result;
	while(y < (int)(height / quadSize))
	{
		std::vector<TerrainQuad> row;
		int x = 0;
		double xOff = 0.0f;
		while(x < (int)(width / quadSize))
		{
			TerrainQuad newQuad(x * quadSize, y * quadSize, quadSize);
			std::array<double, 4> elevations = {{
				nn.noise(xOff, yOff, 0),
				nn.noise(xOff, (y + 1) * yOff, 0),
				nn.noise((x + 1) * xOff, (y + 1) * yOff, 0),
				nn.noise((x + 1) * xOff, y, 0)
			}};
			newQuad.elevations = elevations;
			row.push_back(newQuad);
			x += 1;
			xOff += 0.02;
		}
		result.push_back(row);
		y += 1;
		yOff += 0.02;
	}
	return result;
}

TerrainQuad::TerrainQuad(double posX, double posY, double s)
{
	x = posX;
	y = posY;
	size = s;
}

std::array<std::array<double,3>,4> TerrainQuad::getCorners()
{
	std::array<std::array<double,3>,4> corners = {{
		{x, y, elevations[0]},
		{x, y + size, elevations[1]},
		{x + size, y + size, elevations[2]},
		{x + size, y, elevations[3]}
 	}};
	return corners;
}

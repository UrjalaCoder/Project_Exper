#ifndef TERRAIN
#define TERRAIN
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include "PerlinNoise.h"

class TerrainQuad
{
public:
	double x, y, size;
	std::array<double, 4> elevations = {{0, 0, 0, 0}};
	TerrainQuad(double, double, double);
	std::array<std::array<double,3>,4> getCorners();
};

class TerrainGenerator
{
private:
	PerlinNoise nn;
	int y;
public:
	TerrainGenerator();
	std::vector<std::vector<double>> generate_plane(int width, int height, double z);
	std::vector<std::vector<TerrainQuad>> Generate(int, int, double, double);
};


#endif

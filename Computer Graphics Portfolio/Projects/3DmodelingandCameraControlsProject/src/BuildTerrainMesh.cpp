#include "BuildTerrainMesh.h"

void buildTerrainMesh(ofMesh& terrainMesh, const ofShortPixels& heightmap,
	unsigned int xStart, unsigned int yStart, unsigned int xEnd, unsigned int yEnd, glm::vec3 scale)
{
	using namespace glm;
	terrainMesh.clear();
	// Create vertices
	for (unsigned int y = yStart; y <= yEnd; y++)
	{
		for (unsigned int x = xStart; x <= xEnd; x++)
		{
			
			float h = heightmap.getColor(x, y).r / 65535.0f;
			terrainMesh.addVertex(vec3(x, h, y) * scale);
		}
	}
	// Create indices
	for (unsigned int y = yStart; y < yEnd; y++)
	{

		for (unsigned int x = xStart; x < xEnd; x++)
		{
			unsigned int topLeft = (x - xStart) + (y - yStart) * (xEnd - xStart + 1);
			unsigned int topRight = (x + 1 - xStart) + (y - yStart) * (xEnd - xStart + 1);
			unsigned int bottomLeft = (x - xStart) + (y + 1 - yStart) * (xEnd - xStart + 1);
			unsigned int bottomRight = (x + 1 - xStart) + (y + 1 - yStart) * (xEnd - xStart + 1);

			terrainMesh.addIndex(topLeft);
			terrainMesh.addIndex(bottomLeft);
			terrainMesh.addIndex(topRight);
			terrainMesh.addIndex(topRight);
			terrainMesh.addIndex(bottomLeft);
			terrainMesh.addIndex(bottomRight);
		}
	}
	terrainMesh.flatNormals();
	for (int i = 0; i < terrainMesh.getNumNormals(); ++i)
	{
		terrainMesh.setNormal(i, terrainMesh.getNormal(i) * -1.0f);
	}

}
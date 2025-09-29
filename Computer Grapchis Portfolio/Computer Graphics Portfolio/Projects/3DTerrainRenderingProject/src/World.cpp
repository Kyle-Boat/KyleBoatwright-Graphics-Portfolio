#include "World.h"
#include "buildTerrainMesh.h"

using namespace glm;

float World::getTerrainHeightAtPosition(const glm::vec3& position) const
{
    if (!heightmap)
    {
        return 0.0f;
    }
    else
    {
        // After unscaling, should range between (0, 0, 0) and (1, 1, 1)
        vec3 unscaledPosition { position / dimensions };

        // Remap to the resolution of the heightmap
        vec2 pixelScaledPosition { vec2(unscaledPosition.x, unscaledPosition.z) * vec2(heightmap->getWidth() - 1, heightmap->getHeight() - 1) };

        // Round down and clamp to get pixel indices
        ivec2 baseIndices { clamp(ivec2(floor(pixelScaledPosition)), ivec2(0), ivec2(heightmap->getWidth() - 2, heightmap->getHeight() - 2)) };

        // Calculate linear interpolation weights
        float height00 { static_cast<float>(heightmap->getColor(baseIndices[0],     baseIndices[1])[0]) };
        float height01 { static_cast<float>(heightmap->getColor(baseIndices[0],     baseIndices[1] + 1)[0]) };
        float height10 { static_cast<float>(heightmap->getColor(baseIndices[0] + 1, baseIndices[1])[0]) };
        float height11 { static_cast<float>(heightmap->getColor(baseIndices[0] + 1, baseIndices[1] + 1)[0]) };
        vec2 st { pixelScaledPosition - vec2(baseIndices) };

        // Linearly interpolate and apply the correct scale to the height being returned.
        return (mix(mix(height00, height01, st[1]), mix(height10, height11, st[1]), st[0]) / USHRT_MAX) * dimensions.y; 
    }
}
#pragma once
#include "ofMain.h"

struct DirectionalLight
{
public:
    glm::vec3 direction;
    glm::vec3 color { 1 };
    float intensity { 1 };

    glm::vec3 getDirectionToLight() const
    {
        return normalize(-direction);
    }

    glm::vec3 getColorIntensity() const
    {
        return color * intensity;
    }
};
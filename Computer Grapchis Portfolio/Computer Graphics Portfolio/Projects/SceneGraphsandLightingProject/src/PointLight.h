#pragma once
#include "ofMain.h"

struct PointLight
{
public:
    glm::vec3 position;
    glm::vec3 color { 1 };
    float intensity { 1 };

    glm::vec3 getColorIntensity() const
    {
        return color * intensity;
    }
};

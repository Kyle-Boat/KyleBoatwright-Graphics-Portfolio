#pragma once
#include "PointLight.h"

struct SpotLight : public PointLight
{
public:
    glm::vec3 direction;
    float cutoff;
};

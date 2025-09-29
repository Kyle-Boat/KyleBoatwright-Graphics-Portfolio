#pragma once
#include "DirectionalLight.h"
#include "SpotLight.h"

struct Lighting
{
public:
    glm::vec3 ambientLight;
    DirectionalLight dirLight;
    SpotLight spotLight;
};
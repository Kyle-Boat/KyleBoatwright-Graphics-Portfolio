#pragma once
#include "SimpleDrawNode.h"
#include "Lighting.h"
#include "DirectionalLight.h"
#include "PointLight.h"
class LitDrawNode : public SimpleDrawNode
{
public:
    // Constructor
    LitDrawNode(const ofMesh& mesh, const ofShader& shader, const Lighting& sceneLighting, const glm::vec3& meshColor);

    // Override the base class's draw function
    void drawNode(const CameraMatrices& camera, const glm::mat4& model) const override;

    glm::vec3 meshColor { glm::vec3(1) };
    const Lighting& sceneLighting;
    DirectionalLight dirLight;
    PointLight pLight;
    glm::vec3 ambientLight;
};
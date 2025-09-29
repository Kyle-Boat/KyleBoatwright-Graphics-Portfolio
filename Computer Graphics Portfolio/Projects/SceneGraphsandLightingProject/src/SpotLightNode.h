#pragma once
#include "SceneGraphNode.h"
#include "SpotLight.h"

class SpotLightNode : public SceneGraphNode
{
public:
    SpotLight spotLight;

protected:
    virtual void updateNode(float dt, const glm::mat4& model) override;
};
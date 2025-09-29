#pragma once
#include "SceneGraphNode.h"

class CustomAnimationNode : public SceneGraphNode
{
public:
	CustomAnimationNode(float rotationSpeed, const glm::vec3& rotationAxis);

protected:
	virtual void updateNode(float dt, const glm::mat4& model) override;

private:
	float rotationSpeed;
	glm::vec3 rotationAxis;
	float rotationAngle;
};
#include "SimpleAnimationNode.h"
#include "SimpleAnimationNode.h"

using namespace glm;

SimpleAnimationNode::SimpleAnimationNode(float rotationSpeed, vec3 rotationAxis)
    : rotationSpeed { rotationSpeed }, rotationAxis { rotationAxis }
{
}

void SimpleAnimationNode::updateNode(float dt, const mat4& model)
{
    using namespace glm;
    // Update local transform by appending incremental rotation.
    this->localTransform = rotate(rotationSpeed * dt, rotationAxis) * this->localTransform;
}


#include "SpotLightNode.h"

void SpotLightNode::updateNode(float dt, const glm::mat4& model)
{
    using namespace glm;
    spotLight.position = model[3]; // fourth column: translation; equivalent to model * vec4(0, 0, 0, 1)
    spotLight.direction = -model[2]; // negated third column; equivalent to model * vec4(0, 0, -1, 0);
}

#include "CustomAnimationNode.h"

CustomAnimationNode::CustomAnimationNode(float rotationSpeed, const glm::vec3& rotationAxis)
	: rotationSpeed{ rotationSpeed }, rotationAxis{ rotationAxis }, rotationAngle{ 0.0f }
{

}

void CustomAnimationNode::updateNode(float dt, const glm::mat4& model)
{
	using namespace glm;
	if (degrees(rotationAngle) > -60.0f&&rotationSpeed>0)
	{
		rotationSpeed = -rotationSpeed;
	}
	else if (degrees(rotationAngle) < -120.0f && rotationSpeed < 0)
	{
		rotationSpeed = -rotationSpeed;
	}
	this->rotationAngle += rotationSpeed * dt;
	cout << degrees(rotationAngle) << endl;
	this->localTransform = rotate(rotationSpeed * dt, rotationAxis) * this->localTransform;
}

#include "RobotNPC.h"

using namespace glm;
using namespace std;

RobotNPC::RobotNPC(const World& world) 
    : world { world }
{
}

void RobotNPC::init(float size, float maxSpeed, float maxAccel)
{
    this->size = size;
    this->maxSpeed = maxSpeed;
    this->maxAccel = maxAccel;

	// TODO: Add your own initialization logic.
}

glm::vec3 RobotNPC::getPosition() const
{
    return position;
}

void RobotNPC::setPosition(const glm::vec3& position)
{
    this->position = position;

    // Ensure that the robot is sitting above the terrain.
    float heightFromTerrain { world.getTerrainHeightAtPosition(position) };
    this->position.y = glm::max(position.y, heightFromTerrain);

    // TODO: Add any logic that needs to happen when the robot's position changes.
}

void RobotNPC::setTarget(const glm::vec3& target)
{
    this->target = target;
}

void RobotNPC::update(float dt)
{
    // Physics calculation
    if (dt > 0)
    {
        vec3 displacement { target - position };
        displacement.y = 0;
        float distance { length(displacement) };
        vec3 targetVelocity { displacement * glm::min(glm::max(0.0f, distance - size) / (dt * distance), maxSpeed / distance) };

        vec3 velocityDiff { targetVelocity - velocity };
        velocityDiff.y = 0;
        vec3 accel = velocityDiff * glm::min(1.0f / dt, maxAccel / length(velocityDiff));

        vec3 newVelocity { velocity + (accel + vec3(0, world.gravity, 0)) * dt };
        position += (newVelocity + velocity) * 0.5f * dt;

        float heightFromTerrain { world.getTerrainHeightAtPosition(position) };
        position.y = glm::max(position.y, heightFromTerrain);

        velocity = newVelocity;

		// TODO: Add any logic that needs to happen when the robot's position and velocity change.
    }

    // TODO: Animate the robot here
}

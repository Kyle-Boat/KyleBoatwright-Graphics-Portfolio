#pragma once
#include "ofMain.h"
#include "World.h"

class RobotNPC
{
public:
    // Create a new robot NPC.
    // The world reference allows the robot to be constrained by the height of the terrain.
    RobotNPC(const World& world);

    // Initialize the robot.
    // The "size" of the robot is how much space should be allowed between the robot's "position" and the ground.
    // Max speed and max acceleration constrain the robot's velocity.
    void init(float size = 1.0f, float maxSpeed = 1.0f, float maxAccel = 1.0f);

    // Gets the robot's position
    glm::vec3 getPosition() const;

    // Sets the robot's position.
    void setPosition(const glm::vec3& position);

    // Sets a point in space that the robot is trying to reach.
    void setTarget(const glm::vec3& target);

    // Update the robot: adjust its velocity and animate its parts.
    void update(float dt);
	
    // TODO: Add additional member functions as needed.

private:
    // The world that constrains the robot's position.
    const World& world;

    // The robot's current position.
    glm::vec3 position { 0 };

    // The robot's current velocity.
    glm::vec3 velocity { 0 };

    // The robot's size (the distance from the robot's "position" to the ground).
    float size { 1.0f };

    // The robot's maximum speed, in world-space units per second.
    float maxSpeed { 1.0f };
    
    // The robot's maximum acceleration, in world-space units per second squared.
    float maxAccel { 1.0f };

    // The position of the target the robot is trying to reach.
    glm::vec3 target { 0 };

    // TODO: Add additional member variables as needed.
};

#pragma once
#include <list>
#include <memory>
#include <iostream>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
using namespace glm;
class Billboard {

public:
	glm::vec3 objPos;

	glm::mat4 modelTransform;

	Billboard(glm::vec3 objPos);

	void update(glm::vec3 cameraPos);







};
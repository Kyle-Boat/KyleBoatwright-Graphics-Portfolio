#include "Billboard.h"

Billboard::Billboard(glm::vec3 objPos)
{
  this->objPos = objPos;

  modelTransform = glm::mat4(1.0f);
}

void Billboard::update(glm::vec3 cameraPos)
{
    glm::vec3 differenceInPos = objPos - cameraPos;
  float theta{ glm::atan(differenceInPos.y,differenceInPos.x) };

}

#include "SimpleDrawNode.h"

using namespace glm;

SimpleDrawNode::SimpleDrawNode(const ofMesh& mesh, const ofShader& shader)
    : mesh { mesh }, shader { shader }
{
}

void SimpleDrawNode::drawNode(const CameraMatrices& camera, const glm::mat4& model) const
{
    mat4 mvp { camera.getProj() * camera.getView() * model };
    shader.begin();
    shader.setUniformMatrix4f("mvp", mvp);
    shader.setUniformMatrix3f("normalMatrix", inverse(transpose(model)));
    mesh.draw();
    shader.end();
}

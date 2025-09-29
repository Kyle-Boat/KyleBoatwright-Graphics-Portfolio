#pragma once
#include "SceneGraphNode.h"
#include "ofMain.h"

class SimpleDrawNode : public SceneGraphNode
{
public:
    // Constructor
    SimpleDrawNode(const ofMesh& mesh, const ofShader& shader);

    // Override the base class's draw function
    void drawNode(const CameraMatrices& camera, const glm::mat4& model) const;

protected:
    // The mesh to be drawn (reference to the resource's "home")
    const ofMesh& mesh;

    // The shader to use to draw the mesh (reference to the resource's "home")
    const ofShader& shader;
};
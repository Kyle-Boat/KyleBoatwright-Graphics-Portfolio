#pragma once
#include "ofMain.h"
#include <list>
#include <memory>
#include "CameraMatrices.h"

struct SceneGraphNode
{
public:
    virtual ~SceneGraphNode() {};

    void drawSceneGraph(const CameraMatrices& camera, glm::mat4 globalTransform = {})
    {
        // Apply local transform, then global
        glm::mat4 model { globalTransform * localTransform };

        // Invoke polymorphic draw function
        this->drawNode(camera, model);

        for (auto node : this->childNodes)
        {
            // Recursively drawing child nodes
            // depth-first traversal
            node->drawSceneGraph(camera, model);
        }
    }

    glm::mat4 localTransform {}; // transformation relative to this node's parent

    // List of pointers to the child nodes of this node.
    std::list<std::shared_ptr<SceneGraphNode>> childNodes {};

protected:
    virtual void drawNode(const CameraMatrices& camera, const glm::mat4& model) const
    {
    }
};
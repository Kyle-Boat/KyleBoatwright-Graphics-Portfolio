#pragma once
#include "ofMain.h"
#include <list>
#include <memory>
#include "CameraMatrices.h"

class SceneGraphNode
{
public:
    virtual ~SceneGraphNode() {};

    void updateSceneGraph(float dt, glm::mat4 globalTransform = {})
    {
        if (!updated)
        {
            updated = true; // don't update twice if multiple parents

            // Apply local transform, then global
            glm::mat4 model{ globalTransform * localTransform };

            // Invoke polymorphic update function
            this->updateNode(dt, model);

            for (auto node : this->childNodes)
            {
                // Recursively update child nodes
                // depth-first traversal
                node->updateSceneGraph(dt, model);
            }
        }
    }

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

        // Reset updated flag for next iteration
        updated = false;
    }

    glm::mat4 localTransform {}; // transformation relative to this node's parent

    // List of pointers to the child nodes of this node.
    std::list<std::shared_ptr<SceneGraphNode>> childNodes {};

protected:
    virtual void updateNode(float dt, const glm::mat4& model)
    {
    }

    virtual void drawNode(const CameraMatrices& camera, const glm::mat4& model) const
    {

    }

private:
    bool updated{ false };
};
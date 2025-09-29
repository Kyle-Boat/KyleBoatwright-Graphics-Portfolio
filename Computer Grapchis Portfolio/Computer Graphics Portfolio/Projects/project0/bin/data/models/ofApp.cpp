#include "ofApp.h"
#include "GLFW/glfw3.h"
#include "CameraMatrices.h"
#include "SimpleDrawNode.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    using namespace glm;

    ofDisableArbTex(); // IMPORTANT!
    
    // Disable Windows cursor
    auto window { ofGetCurrentWindow() };
    //glfwSetInputMode(dynamic_pointer_cast<ofAppGLFWWindow>(window)
    //    ->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ofEnableDepthTest();

    // Load shaders for the first time
    reloadShaders();

    // Load torus mesh
    torusMesh.load("models/cube.ply");

    

    //for (size_t i { 0 }; i < torusMeshFlat.getNumNormals(); i++)
    //{
    //    torusMeshFlat.setNormal(i, -torusMeshFlat.getNormal(i));
    //}

    //// Initialize scene graph

    //// add non-drawing node to represent the rotating torus
    sceneGraphRoot.childNodes.emplace_back(new SceneGraphNode {});

    //// rotating torus node
    //// torus node is the most recent node added to the scene graph at this point
    torusNode = sceneGraphRoot.childNodes.back();

	//// Set the local transform of the torus node to rotate around the y-axis
	// torusNode->localTransform = translate(vec3(0, 0, 0)) * rotate(PI / 2, vec3(1, 0, 0));
    // 
    //// node that actually draws the torus
    torusNode->childNodes.emplace_back(new SimpleDrawNode { torusMesh, shader });
	torusNode->childNodes.back() // node just added
		->localTransform = translate(vec3(0, -1, 0)) * scale(vec3(20,0.02,20));
    auto torusMeshNode = torusNode->childNodes.back();

    //// Add non-drawing node as child of rotating torus node
    //torusNode->childNodes.emplace_back(new SceneGraphNode {});
    //torusNode->childNodes.back() // node just added
    //    ->localTransform = translate(vec3(0, 1.5, 0)) * scale(vec3(0.5));
    //torusNode->childNodes.back() // node just added
    //    ->childNodes.push_back(torusMeshNode);

    //// add joint node as child of rotating torus node
    //torusNode->childNodes.emplace_back(new SceneGraphNode {});
    //jointNode = torusNode->childNodes.back();
    //jointNode->localTransform =
    //    translate(vec3(0, -1, 0)) /* rotation here */;
    //
    //jointNode->childNodes.emplace_back(new SceneGraphNode {});

    //// Translate so upper edge is at origin
    //jointNode->childNodes.back()->localTransform = translate(vec3(0, -1, 0)); 

    //// Make torus mesh a child of the translation node
    //jointNode->childNodes.back()->childNodes.push_back(torusMeshNode);


    //// add joint2 as child of joint
    //jointNode->childNodes.emplace_back(new SceneGraphNode {});
    //jointNode2 = jointNode->childNodes.back();
    //jointNode2->localTransform =
    //    translate(vec3(0, -2, 0)) /* rotation here */;

    //jointNode2->childNodes.emplace_back(new SceneGraphNode {});

    //// Translate so upper edge is at origin
    //jointNode2->childNodes.back()->localTransform = translate(vec3(0, -1, 0));

    //// Make torus mesh a child of the translation node
    //jointNode2->childNodes.back()->childNodes
    //    .emplace_back(new SimpleDrawNode { torusMeshFlat, shader });


    //// Add non-drawing node to scene graph
    //sceneGraphRoot.childNodes.emplace_back(new SceneGraphNode {});

    //sceneGraphRoot.childNodes.back() // node just added
    //    ->childNodes.push_back(torusNode); // Add a pointer to the torus node as a child

    //sceneGraphRoot.childNodes.back() // node just added
    //    ->localTransform = translate(vec3(3, 0, 0));

    //// Add non-drawing node to scene graph
    //sceneGraphRoot.childNodes.emplace_back(new SceneGraphNode {});

    //sceneGraphRoot.childNodes.back() // node just added
    //    ->childNodes.push_back(torusMeshNode);

    //sceneGraphRoot.childNodes.back() // node just added
    //    ->localTransform = translate(vec3(-3, 0, 0));

    // Initialize quad for billboarding
    quad.addVertex(vec3(-2, -1, 0));
    quad.addVertex(vec3(-2, 1, 0));
    quad.addVertex(vec3(2, 1, 0));
    quad.addVertex(vec3(2, -1, 0));

    quad.addTexCoord(vec2(0, 0));
    quad.addTexCoord(vec2(0, 1));
    quad.addTexCoord(vec2(1, 1));
    quad.addTexCoord(vec2(1, 0));

    ofIndexType indices[6] = { 0, 1, 2, 2, 3, 0 };
    quad.addIndices(indices, 6);

    // Load image for billboarding
    img.load("textures/drwho.jpg");
    img.getTexture().generateMipmap();
    img.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

void ofApp::reloadShaders()
{
    shader.load("shaders/my.vert", "shaders/my.frag");
    textureShader.load("shaders/my.vert", "shaders/texture.frag");
    needsReload = false;
}

void ofApp::updateCameraRotation(float dx, float dy)
{
    using namespace glm;

    cameraPitch -= dy;
    // -89 to +89 degrees, converted to radians
    cameraPitch = glm::clamp(cameraPitch, radians(-89.0f), radians(89.0f)); 

    cameraHead -= dx;
}

void ofApp::updateTorusRotation(float dx, float dy)
{
    using namespace glm;
    mat3 currentRotation { mat3(torusNode->localTransform) };
    vec3 currentTranslation { torusNode->localTransform[3] };
    torusNode->localTransform = translate(currentTranslation) 
        * rotate(dx, vec3(0, 1, 0)) * mat4(currentRotation);
}

void ofApp::updateJointRotation(float dx, float dy)
{
    using namespace glm;
    mat3 currentRotation { mat3(jointNode->localTransform) };
    vec3 currentTranslation { jointNode->localTransform[3] };
    jointNode->localTransform = translate(currentTranslation)
        * rotate(dy, vec3(1, 0, 0)) * mat4(currentRotation);
}

void ofApp::updateJoint2Rotation(float dt)
{
    using namespace glm;
    mat3 currentRotation { mat3(jointNode2->localTransform) };
    vec3 currentTranslation { jointNode2->localTransform[3] };
    jointNode2->localTransform = translate(currentTranslation)
        * rotate(dt, vec3(0, 1, 0)) * mat4(currentRotation);
}

//--------------------------------------------------------------
void ofApp::update()
{
    if (needsReload)
    {
        reloadShaders();
    }

    float dt { static_cast<float>(ofGetLastFrameTime()) };

    // Update position using velocity and dt.
    using namespace glm;
    camera.position += mat3(rotate(cameraHead, vec3(0, 1, 0))) 
        * velocity * dt;
    camera.rotation = rotate(cameraHead, vec3(0, 1, 0)) *
        rotate(cameraPitch, vec3(1, 0, 0));
        
    /*updateJoint2Rotation(dt);*/
}

glm::mat4 screenAlignedBillboard(glm::mat4 modelView)
{
    using namespace glm;
    vec3 camSpacePos { modelView * vec4(0, 0, 0, 1) };
    return translate(camSpacePos);
}

glm::mat4 viewpointOrientedBillboard(glm::mat4 modelView, glm::vec3 up)
{
    using namespace glm;
    vec3 camSpacePos { modelView * vec4(0, 0, 0, 1) };
    vec3 n { -normalize(camSpacePos) };
    vec3 r { normalize(cross(up, n)) };
    vec3 u { normalize(cross(n, r)) };
    return mat4(vec4(r, 0), vec4(u, 0), vec4(n, 0), vec4(camSpacePos, 1))
        * scale(vec3(sqrt(determinant(mat3(modelView)))));
}

//--------------------------------------------------------------
void ofApp::draw()
{
    using namespace glm;

    float width { static_cast<float>(ofGetViewportWidth()) };
    float height { static_cast<float>(ofGetViewportHeight()) };
    float aspect = width / height;

    // Calculate the view and projection matrices for the camera.
    CameraMatrices camMatrices { camera, aspect, 0.01f, 10.0f, };

    shader.begin(); // make shader active just to set uniform values
    shader.setUniform3f("lightDir", normalize(vec3(1, 1, 1)));
    shader.setUniform3f("lightColor", vec3(1, 1, 1)); // white light
    shader.setUniform3f("meshColor", vec3(1, 1, 1)); // white material
    shader.setUniform3f("ambientColor", vec3(0.1, 0.1, 0.1));
    //.draw();
    shader.end();

    sceneGraphRoot.drawSceneGraph(camMatrices);

    textureShader.begin();
    textureShader.setUniformMatrix4f("mvp",
        camMatrices.getProj() * viewpointOrientedBillboard(camMatrices.getView() * translate(vec3(0, 0, 5)),vec3(0,1,0)));
    textureShader.setUniformTexture("tex", img, 0);
    quad.draw();
    textureShader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    using namespace glm;

    if (key == OF_KEY_UP || key == 'w')
    {
        // Forward motion
        // Converting from camera space velocity to world space velocity
        velocity.z = -1;
    }
    else if (key == OF_KEY_DOWN || key == 's')
    {
        // Backwards motion
        // Converting from camera space velocity to world space velocity
        velocity.z = 1;
    }
    else if (key == OF_KEY_LEFT || key == 'a')
    {
        // Forward motion
        // Converting from camera space velocity to world space velocity
        velocity.x = -1;
    }
    else if (key == OF_KEY_RIGHT || key == 'd')
    {
        // Backwards motion
        // Converting from camera space velocity to world space velocity
        velocity.x = 1;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (key == '`')
    {
        // Reload shaders
        needsReload = true;
    }
    else if (key == OF_KEY_LEFT || key == 'a' || key == OF_KEY_RIGHT || key == 'd')
    {
        // Reset velocity when a key is released
        velocity.x = 0;
    }
    else if (key == OF_KEY_UP || key == 'w' || key == OF_KEY_DOWN || key == 's')
    {
        // Reset velocity when a key is released
        velocity.z = 0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
    if (prevX != 0 && prevY != 0)
    {
        // Previous mouse position has been initialized.
        // Calculate dx and dy
        int dx = x - prevX;
        int dy = y - prevY;

        updateCameraRotation(mouseSensitivity * dx, mouseSensitivity * dy);
    }

    prevX = x;
    prevY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
    if (prevX != 0 && prevY != 0)
    {
        // Previous mouse position has been initialized.
        // Calculate dx and dy
        int dx = x - prevX;
        int dy = y - prevY;

        if (button == 0) // left mouse button: rotate torus
        {
            updateTorusRotation(mouseSensitivity * dx, mouseSensitivity * dy);
        }
        else if (button == 2) // right mouse button: rotate joint
        {
            updateJointRotation(mouseSensitivity * dx, mouseSensitivity * dy);
        }
    }

    prevX = x;
    prevY = y;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{

}

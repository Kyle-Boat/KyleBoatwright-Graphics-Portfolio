#include "ofMain.h"
#include "ofApp.h"
const unsigned int PORTRAIT_COUNT{ 14 };
const glm::vec2 UV_SCALE{ glm::vec2(1.0 / 7.0, 0.5) };

const glm::vec2 UV_OFFSETS[PORTRAIT_COUNT]
{
    glm::vec2(0, 0), glm::vec2(1. / 7, 0), glm::vec2(2. / 7, 0), glm::vec2(3. / 7, 0),
    glm::vec2(4. / 7, 0), glm::vec2(5. / 7, 0), glm::vec2(6. / 7, 0),
    glm::vec2(0, .5), glm::vec2(1. / 7, .5), glm::vec2(2. / 7, .5), glm::vec2(3. / 7, .5),
    glm::vec2(4. / 7, .5), glm::vec2(5. / 7, .5), glm::vec2(6. / 7, .5)
};

// Length of time a single portrait is on screen
const float MAX_LIFE{ 6.0f };

//--------------------------------------------------------------
void ofApp::setup()
{
    using namespace glm;
    //Use this instead of name glm constantly but I did it in line 10
    // IMPORTANT!  Don't remove this line or textures won't work:
    ofDisableArbTex();
    quad.addVertex(glm::vec3(-1, 1, 0.0));
    quad.addColor(ofFloatColor(1, 0, 0));
    quad.addTexCoord(vec2(0, 0));

    quad.addVertex(glm::vec3(-1, -1, 0.0));
    quad.addColor(ofFloatColor(0, 1, 0));
    quad.addTexCoord(vec2(0, 1));

    quad.addVertex(glm::vec3(1, -1, 0.0));
    quad.addColor(ofColor(0, 0, 1));
    quad.addTexCoord(vec2(1, 1));

    quad.addVertex(glm::vec3(1, 1, 0.0));
    quad.addColor(ofColor(0, 0, 1));
    quad.addTexCoord(vec2(1, 0));

    ofIndexType indices[6] = { 0, 1, 2, 2, 3, 0 };
    quad.addIndices(indices, 6);

    img.load("textures/MajorasMask.jpg");

   /* triangle.addVertex(glm::vec3(0.0, 0.0, 0.0));
    triangle.addColor(ofColor(1, 0, 0));
    triangle.addVertex(glm::vec3(0.0, 769.0, 0.0));
    triangle.addColor(ofColor(0, 1, 0));
    triangle.addVertex(glm::vec3(1024.0, 769.0, 0.0));
    triangle.addColor(ofColor(0, 0, 1));*/
    //shader.load()
}

void ofApp::reloadShaders()
{
    // Load shaders here:
    shader.load("shaders/my.vert", "shaders/my.frag"); 
}

//--------------------------------------------------------------
void ofApp::update()
{
    // First, check for shader reload via hotkey:
    if (needsReload)
    {
        reloadShaders();
        needsReload = false;
    }

    // Add additional update logic as needed:
    float dt = ofGetLastFrameTime();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    using namespace glm;
    shader.begin();
    shader.setUniformTexture("tex", img, 0);
    quad.draw();
    shader.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // Shader reload hotkey
    if (key == '`')
    {
        needsReload = true;
    }

    if (key == ' ')
    {
        //triangleColor = glm::vec3(0.25, 1.0, 0.75);
    }
    // Handle other key press events as needed:

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{

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

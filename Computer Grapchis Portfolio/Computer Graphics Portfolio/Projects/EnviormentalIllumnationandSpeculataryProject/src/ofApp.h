#pragma once
#include "ofMain.h"
#include "ofxCubemap.h"
#include "CameraMatrices.h"
class ofApp : public ofBaseApp
{
public:
    void drawCube(const CameraMatrices& camMatrices, ofShader& skyboxShader, ofMesh& cubeMesh);
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    // world texture
    ofImage worldTex;

    Camera camera;
    // metal texture
    ofImage metalTex;
    //3D perspectiveshader
    ofShader shader;
    //mesh of hammer
    ofMesh hammerMesh;

    //diffuse (color) texture
    ofImage hammerDiffuse{};

	ofImage hammerSpecular{};

    //normal map
    ofImage hammerNormal{};

    // Torus mesh object
    ofMesh torusMesh;

    // Cube mesh object
    ofMesh cubeMesh;

    // Circle mesh object
    ofMesh circleMesh;

    // Cylinder mesh object
    ofMesh cylinderMesh;

    // Sphere mesh object
    ofMesh sphereMesh;

    ofShader skyboxShader;

    ofxCubemap cubeMap;

	ofImage irradianceMap;
    // Do shaders need to be reloaded?
    bool needsReload{ true };

    // Load the shaders for this app
    void reloadShaders();

    // (x, y) from the previous frame
    int prevX{ 0 }, prevY{ 0 };

    // How many radians of rotation correspond to a single pixel of movement of the cursor.
    float mouseSensitivity{ 0.02f };

    // The current head direction of the camera in radians.
    float cameraHead{ 0 };

    // The current pitch angle of the camera in radians.
    float cameraPitch{ 0 };

    // Velocity of the camera (from WASD) -- in camera space
    glm::vec3 velocity{ };

    // Position of the camera in world space
    glm::vec3 position{ 0, 0, 0 };

    // update camera rotation based on mouse movement
    void updateCameraRotation(float dx, float dy);
};

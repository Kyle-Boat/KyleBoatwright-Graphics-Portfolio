#pragma once

#include "ofMain.h"
#include "CellManager.h"
class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();
	void exit();

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
	// 3D perspective shader
	ofShader shader;

	// Torus mesh object
	ofVboMesh torusMesh;

	// (x, y) from the previous frame
	int prevX{ 0 }, prevY{ 0 };

	// How many radians of rotation correspond to a single pixel of movement of the cursor.
	float mouseSensitivity{ 0.02f };

	// The current head direction of the camera in radians.
	float cameraHead{ 2.4f};

	// The current pitch angle of the camera in radians.
	float cameraPitch{ 0 };

	// Velocity of the camera (from WASD) -- in camera space
	glm::vec3 velocity{ };

	// Position of the camera in world space
	glm::vec3 position{ 0, 200, 2 };

	// update camera rotation based on mouse movement
	void updateCameraRotation(float dx, float dy);

	// Variables go here
	ofVboMesh cubeMesh{};
	void buildCube(ofMesh& mesh);
	ofVboMesh sphereMesh{};
	void buildSphere(ofMesh& mesh, int subdivTheta, int subdivPhi);
	ofVboMesh cylinderMesh{};
	void buildCylinder(ofMesh& mesh, int subdiv);

	ofVboMesh terrainMesh;
	ofShortImage heightmapHiRes{};
	CellManager<5> cellManager{ heightmapHiRes, 1600, 256 };
	ofShortImage heightmap{};
	ofShader shader2{};
	ofShortImage watermap{};
	ofVboMesh waterMesh{};
	
	void buildCircle(ofMesh& mesh, int subdiv);

	


























	// Shader reload hotkey:
	bool needsReload{ true };
	void reloadShaders();
};

#pragma once
#include "ofMain.h"
#include "../MySceneGraph.h"
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

	MySceneGraph sceneGraph{};
	// (x, y) from the previous frame
	int prevX{ 0 }, prevY{ 0 };

	// How many radians of rotation correspond to a single pixel of movement of the cursor.
	float mouseSensitivity{ 0.02f };

	// The current head direction of the camera in radians.
	float cameraHead{ 0.0f };

	// The current pitch angle of the camera in radians.
	float cameraPitch{ 0 };

	// Velocity of the camera (from WASD) -- in camera space
	glm::vec3 velocity{ };

	Camera camera{};
	// update camera rotation based on mouse movement
	void updateCameraRotation(float dx, float dy);


	// Shader reload hotkey:
	bool needsReload{ true };
	void reloadShaders();
};

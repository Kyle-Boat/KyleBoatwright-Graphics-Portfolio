#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp
{
public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
		
private:
	// Variables go here
	ofMesh sphere;
	ofMesh block;
	ofMesh torus;
	ofMesh mesh;
	ofMesh myMesh;
	ofShader shader;
	float view;
	float rotation;
	float verticalRotation;
	ofVbo myVBO;
	/*ofMesh torusMesh{};*/

	glm::vec3 meshPosition;
	float WSMove = -3.0f;
	float ADMove = 0.0f;
	float QEMove = 0.0f;
	/*ofVbo torusVBO{};
	float torusrotation{ 0 };

	ofShader shader;*/
	// Shader reload hotkey:
	bool needsReload { true };
	void reloadShaders();
	// (x, y) from the previous frame
	int prevX{ 0 }, prevY{ 0 };

	//// How many radians of rotation correspond to a single pixel of movement of the cursor.
	float mouseSensitivity{ 0.02f };

	//// The current head direction of the camera in radians.
	float cameraHead{ 0 };
	float cameraPitch{ 0 };
	//// Velocity of the camera (from WASD) -- in camera space
	glm::vec3 velocity{ };

	//// Position of the camera in world space
	glm::vec3 position{ 0, 0, 1 };
	ofShortImage heightmap;
	ofVboMesh terrainMesh;

	void updateCameraRotation(float dx, float dy);
};

#pragma once

#include "ofMain.h"
#include "Camera.h"
#include "SceneGraphNode.h"

class ofApp : public ofBaseApp
{

public:
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
	ofShader shader;

	// Torus mesh object with flat shading
	ofMesh torusMeshFlat;

	// Torus mesh object with smooth shading
	ofMesh torusMesh;

	// Do shaders need to be reloaded?
	bool needsReload { true };

	// Load the shaders for this app
	void reloadShaders();

	// Keep track of previous mouse position
	int prevX { 0 }, prevY { 0 };

	// Allows us to configure how much moving the mouse affects the rotation
	float mouseSensitivity { 0.02f };

	// Euler transformation
	float cameraHead { 0 };
	float cameraPitch { 0 };

	// Camera position
	Camera camera { glm::vec3(0, 0, 2) };
	glm::vec3 velocity {};

	// Root node of the scene graph
	SceneGraphNode sceneGraphRoot {};
	std::shared_ptr<SceneGraphNode> torusNode {};
	std::shared_ptr<SceneGraphNode> jointNode {};
	std::shared_ptr<SceneGraphNode> jointNode2 {};

	// Quad mesh for billboarding
	ofMesh quad;

	// Image for billboarding
	ofImage img;

	// Shader for billboarding
	ofShader textureShader;

	// Called to update rotation of the camera from mouse movement
	void updateCameraRotation(float dx, float dy);

	// Called to update rotation of the torus base from left-mouse-button drag
	void updateTorusRotation(float dx, float dy);

	// Called to update rotation of the torus base from right-mouse-button drag
	void updateJointRotation(float dx, float dy);

	// Called to update rotation of the torus base from right-mouse-button drag
	void updateJoint2Rotation(float dt);
};

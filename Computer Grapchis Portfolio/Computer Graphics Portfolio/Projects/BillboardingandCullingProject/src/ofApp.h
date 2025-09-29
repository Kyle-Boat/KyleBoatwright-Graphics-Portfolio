#pragma once

#include "ofMain.h"
#include "Camera.h"
#include "SceneGraphNode.h"
#include "ParticleSystem.h"
#include "BasicParticleGenerator.h"

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
	// Do shaders need to be reloaded?
	bool needsReload { true };

	// Load the shaders for this app
	void reloadShaders();

	// Keep track of previous mouse position
	int prevX { 0 }, prevY { 0 };

	// Allows us to configure how much moving the mouse affects the rotation
	float mouseSensitivity { 0.02f };

	// Euler transformation
	float cameraHead { 180 };
	float cameraPitch { 0 };

	bool BehindCameraCulling = false;
	bool FrustumCulling2D = false;
	bool culled = false;
	bool FullFrustumCulling = false;
	int xbyx = 1;

	// Camera position
	Camera camera { glm::vec3(0, 0, 2) };
	glm::vec3 velocity {};

	SceneGraphNode sceneGraphRoot {};
	std::shared_ptr<SceneGraphNode> CubeNode {};
	std::shared_ptr<SceneGraphNode> jointNode {};
	std::shared_ptr<SceneGraphNode> jointNode2 {};

	
	unsigned int currentPng{ 1 };

	
	float timeSinceRegen{ 0 };

	
	float blendParam{ 0 };

	float brightness{ 1.0f };



	ofMesh cubeMesh;

	ofMesh fireQuad;
	ofMesh smokeQuad;
	ofMesh skyQuad;


	ofImage skyImg;
	ofImage smokeImg;
	ofImage fireImg;

	ofShader shader;
	ofShader smokeShader;
	ofShader fireShader;

	BasicParticleGenerator particleGenerator{};
	ParticleSystem<BasicParticle> particleSystem{ particleGenerator, 40, 5.0f };

	void updateCameraRotation(float dx, float dy);

};

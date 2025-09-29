#pragma once

#include "ofMain.h"
#include "BasicParticleGenerator.h"
#include"ParticleSystem.h"

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
	//BasicPae
	// Shader reload hotkey:

	BasicParticleGenerator particleGenerator{};
	ParticleSystem<BasicParticle> particleSystem{ particleGenerator, 42,20.0f };
	ParticleSystem<BasicParticle> particleSystem2{ particleGenerator, 42,20.0f };
	ofMesh quad{};
	ofShader shader{};
	ofShader shader2{};
	ofImage img{};
	ofImage img2{};
	ofImage img3{};
	ofMesh quad2{};

	bool needsReload { true };
	void reloadShaders();
};

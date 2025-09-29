#pragma once
#include "SceneGraphNode.h"
#include "Lighting.h"
#include "SpotLightNode.h"
class MySceneGraph {
public:
	void setup(ofShader& shader);
	void update(float dt);
	void draw(const CameraMatrices& camera);

private:
	SceneGraphNode rootNode{};

	// Torus mesh object
	ofVboMesh cubeMesh{};
	ofVboMesh sphereMesh{};
	ofVboMesh torusMesh{};

	Lighting lighting{};
	
	shared_ptr<SpotLightNode> spotlightNode{};
};
#include "ofApp.h"
#include "buildTerrainMesh.h"
using namespace glm;
//--------------------------------------------------------------
void ofApp::setup()
{
	// IMPORTANT!  Don't remove this line or textures won't work:
	ofDisableArbTex();
	ofEnableAlphaBlending();
	ofEnableDepthTest();
	glEnable(GL_CULL_FACE);
	ofSetBackgroundColor(180,180,255);
	heightmap.setUseTexture(false);
	heightmap.load("TamrielLowRes.png");
	heightmapHiRes.setUseTexture(false);
	heightmapHiRes.load("TamrielBeta_10_2016_01.png");
	watermap.load("RandomLowRes.png");
	watermap.setUseTexture(false);
	float scale{ (heightmapHiRes.getHeight() - 1) / (heightmap.getHeight() - 1) };
	float scale2{ (heightmapHiRes.getHeight() - 1 )/ (watermap.getHeight() - 1) };
	position = vec3(
		(heightmapHiRes.getWidth() - 1) * 0.5f,
		850, // or 420 if using random noise heightmap
		(heightmapHiRes.getHeight() - 1) * 0.5f);

	cellManager.initializeForPosition(position);
	assert(heightmap.getWidth() != 0 && heightmap.getHeight() != 0);
	cout << heightmap.getWidth() << endl;
	cout << heightmap.getHeight() << endl;
	buildTerrainMesh(terrainMesh,heightmap.getPixels(),10,10, heightmap.getWidth() - 1, heightmap.getHeight() - 1, vec3(scale, 50*scale, scale));
	
	buildTerrainMesh(waterMesh, watermap.getPixels(), 10, 10, watermap.getWidth() - 1, watermap.getHeight() - 1, vec3(scale2, scale2, scale2));
	for (int i = 0; i < waterMesh.getNumVertices(); ++i)
	{
		glm::vec3 vertex = waterMesh.getVertex(i);
		vertex.y += 700;
		waterMesh.setVertex(i, vertex);
	}
}

void ofApp::reloadShaders()
{
	// Load shaders here:
	shader.load("shaders/my.vert", "shaders/my2.frag");
	shader2.load("shaders/my.vert", "shaders/my.frag");
}

void ofApp::updateCameraRotation(float dx, float dy)
{
	using namespace glm;
	cameraHead += dx;
	cameraPitch += dy;
	cameraPitch = glm::clamp(cameraPitch, -static_cast<float>(PI) / 2, static_cast<float>(PI) / 2);
}

void ofApp::buildCube(ofMesh& mesh)
{
	using namespace glm;
	mesh.addVertex(vec3(-1, -1, -1));// left  bottom back
	mesh.addVertex(vec3(-1, -1, 1)); // left  bottom front
	mesh.addVertex(vec3(-1, 1, -1));//  left  up     back
	mesh.addVertex(vec3(-1, 1, 1)); //  left  up     front
	mesh.addVertex(vec3(1, -1, -1));//  right bottom back
	mesh.addVertex(vec3(1, -1, 1));//   right bottom front
	mesh.addVertex(vec3(1, 1, -1)); //  right top    back
	mesh.addVertex(vec3(1, 1, 1)); //   right top    front

	ofIndexType indices[36] =
	{
		2,3,7,2,7,6, // top
		3,1,5,3,5,7, // front
		7,5,4,7,4,6, // right
		1, 0 ,5, 0, 4, 5,// bottom
		6,4,0,6,0,2, // back
		2,0,3,0,1,3 //  left
	};
	mesh.addIndices(indices, 36);
	mesh.flatNormals();
}
void ofApp::buildSphere(ofMesh& mesh, int subdivTheta, int subdivPhi)
{
	using namespace glm;

	// top center vertex: north pole
	mesh.addVertex(vec3(0, 1, 0));

	// top outer ring
	for (int i{ 1 }; i < subdivTheta; i++)
	{
		double theta{ i * PI / subdivTheta };

		for (int j{ 0 }; j < subdivPhi; j++)
		{
			double phi{ j * 2 * PI / subdivPhi };

			mesh.addVertex(vec3(sin(theta) * cos(phi), cos(theta), -sin(theta) * sin(phi)));
		}
	}

	// bottom center vertex: south pole
	mesh.addVertex(vec3(0, -1, 0));


	int northPoleRingStart{ 1 };

	// index buffer -- top ring
	for (int i{ 0 }; i < subdivPhi; i++)
	{
		mesh.addIndex(0);
		mesh.addIndex(i + northPoleRingStart);
		mesh.addIndex((i + 1) % subdivPhi + northPoleRingStart);
	}

	int southPole{ (subdivTheta - 1) * subdivPhi + 1 };
	int southPoleRingStart{ southPole - subdivPhi };

	// index buffer -- bottom ring
	for (int i{ 0 }; i < subdivPhi; i++)
	{
		// winding order flipped to face downwards
		mesh.addIndex(southPole);
		mesh.addIndex((i + 1) % subdivPhi + southPoleRingStart);
		mesh.addIndex(i + southPoleRingStart);
	}

	// index buffer -- sides
	for (int i{ 0 }; i < subdivTheta - 2; i++)
	{
		int topRingStart = 1 + subdivPhi * i;
		int bottomRingStart{ topRingStart + subdivPhi };

		for (int j{ 0 }; j < subdivPhi; j++)
		{
			// indices for the quad
			ofIndexType quadIndices[4]
			{
				j + topRingStart,
				(j + 1) % subdivPhi + topRingStart,
				j + bottomRingStart,
				(j + 1) % subdivPhi + bottomRingStart,
			};

			// first triangle
			mesh.addIndex(quadIndices[0]);
			mesh.addIndex(quadIndices[2]);
			mesh.addIndex(quadIndices[3]);

			// second triangle
			mesh.addIndex(quadIndices[0]);
			mesh.addIndex(quadIndices[3]);
			mesh.addIndex(quadIndices[1]);
		}
	}

	mesh.flatNormals();
}



void ofApp::buildCylinder(ofMesh& mesh, int subdiv)
{
	using namespace glm;

	// top center vertex
	mesh.addVertex(vec3(0, 1, 0));

	// top outer ring
	for (int i{ 0 }; i < subdiv; i++)
	{
		double theta{ i * 2 * PI / subdiv };
		mesh.addVertex(vec3(cos(theta), 1, -sin(theta)));
	}

	// bottom center vertex
	mesh.addVertex(vec3(0, -1, 0));

	// bottom outer ring
	for (int i{ 0 }; i < subdiv; i++)
	{
		double theta{ i * 2 * PI / subdiv };
		mesh.addVertex(vec3(cos(theta), -1, -sin(theta)));
	}

	int topRingStart{ 1 };

	// index buffer -- top ring
	for (int i{ 0 }; i < subdiv; i++)
	{
		mesh.addIndex(0);
		mesh.addIndex(i + topRingStart);
		mesh.addIndex((i + 1) % subdiv + topRingStart);
	}

	int bottomCenter{ subdiv + 1 };
	int bottomRingStart{ bottomCenter + 1 };

	// index buffer -- bottom ring
	for (int i{ 0 }; i < subdiv; i++)
	{
		// winding order flipped to face downwards
		mesh.addIndex(bottomCenter);
		mesh.addIndex((i + 1) % subdiv + bottomRingStart);
		mesh.addIndex(i + bottomRingStart);
	}

	// index buffer -- sides
	for (int i{ 0 }; i < subdiv; i++)
	{
		// indices for the quad
		ofIndexType quadIndices[4]
		{
			i + topRingStart,
			(i + 1) % subdiv + topRingStart,
			i + bottomRingStart,
			(i + 1) % subdiv + bottomRingStart,
		};

		// first triangle
		mesh.addIndex(quadIndices[0]);
		mesh.addIndex(quadIndices[1]);
		mesh.addIndex(quadIndices[2]);

		// second triangle
		mesh.addIndex(quadIndices[1]);
		mesh.addIndex(quadIndices[2]);
		mesh.addIndex(quadIndices[3]);
	}

	mesh.flatNormals();
}


void ofApp::buildCircle(ofMesh& mesh, int subdiv)
{
	using namespace glm;
	//center vertex
	mesh.addVertex(vec3(0, 0, 0));
	//outer ring
	for (int i{ 0 }; i < subdiv; i++) {
		double theta{ i * 2 * PI / subdiv };
		mesh.addVertex(vec3(cos(theta), sin(theta), 0));
	}
	int outerRingStart{ 1 };
	//index buffer
	for (int i{ 1 }; i <= subdiv; i++) {

		mesh.addIndex(0);
		mesh.addIndex(i + outerRingStart);
		mesh.addIndex((i + 1) % subdiv + outerRingStart);
	}
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

	using namespace glm;

	// calculate world space velocity
	vec3 velocityWorldSpace{ mat3(rotate(-cameraHead, vec3(0, 1, 0))) * velocity };

	// time since last frame
	float dt{ static_cast<float>(ofGetLastFrameTime()) };

	// update position
	position += velocityWorldSpace * dt;

	cellManager.optimizeForPosition(position);



}

//--------------------------------------------------------------
void ofApp::draw()
{
	using namespace glm;

	float width{ static_cast<float>(ofGetViewportWidth()) };
	float height{ static_cast<float>(ofGetViewportHeight()) };
	float aspect{ width / height };
	float farPlane{ 800.0 };
	mat4 model{  translate(vec3(0,0,0))};
	mat4 view{ rotate(cameraPitch, vec3(1, 0, 0)) * rotate(cameraHead, vec3(0, 1, 0)) * translate(-position) };
	mat4 proj{ perspective(radians(90.0f), aspect, 0.01f, 2000.0f) };

	mat4 mvp{ proj * view * model };

	shader.begin();
	
	shader.setUniformMatrix4f("mvp", mvp);

	shader.setUniformMatrix4f("MV", view * model);
	shader.setUniform3f("meshColor", vec3(1, 0.1f, 0.1f));
	shader.setUniform3f("lightColor", vec3(8));
	shader.setUniform3f("lightDirection", normalize(vec3(1, 1, 1)));
	shader.setUniformMatrix3f("normalMatrix", mat3(inverse(transpose(model))));



	terrainMesh.draw();
	shader.setUniform3f("meshColor", vec3(0.1, 0.1f, 1.0f));
	waterMesh.draw();
	shader.end();
	glClear(GL_DEPTH_BUFFER_BIT);
	shader2.begin();
	shader2.setUniformMatrix4f("mvp", mvp);

	shader2.setUniformMatrix4f("MV", view * model);
	shader2.setUniform3f("meshColor", vec3(1, 0.1f, 0.1f));
	shader2.setUniform3f("lightColor", vec3(8));
	shader2.setUniform3f("lightDirection", normalize(vec3(1, 1, 1)));
	shader2.setUniform3f("cameraPos", position);
	shader2.setUniformMatrix3f("normalMatrix", mat3(inverse(transpose(model))));

	cellManager.drawActiveCells(position, farPlane);
	shader2.end(); 
}

//--------------------------------------------------------------
void ofApp::exit()
{
	cellManager.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

	float moveSense = 50.0f;
	// Shader reload hotkey
	if (key == '`')
	{
		needsReload = true;
	}
	else if (key == 'w')
	{
		velocity.z = -moveSense;
	}
	else if (key == 's')
	{
		velocity.z = moveSense;
	}
	else if (key == 'a')
	{
		velocity.x = -moveSense;
	}
	else if (key == 'd')
	{
		velocity.x = moveSense;
	}
	else if (key == 'e')
	{
		velocity.y = moveSense;
	}
	else if (key == 'q')
	{
		velocity.y = -moveSense;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	if (key == 'w' || key == 's')
	{
		velocity.z = 0;
	}
	else if (key == 'a' || key == 'd')
	{
		velocity.x = 0;
	}
	else if (key == 'e' || key == 'q')
	{
		velocity.y = 0;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
	if (prevX != 0 && prevY != 0)
	{
		// Update camera rotation based on mouse movement
		updateCameraRotation(mouseSensitivity * (x - prevX), mouseSensitivity * (y - prevY));
	}

	// Remember where the mouse was this frame.
	prevX = x;
	prevY = y;
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


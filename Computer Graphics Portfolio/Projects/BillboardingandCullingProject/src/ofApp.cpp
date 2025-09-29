#include "ofApp.h"
#include "GLFW/glfw3.h"
#include "CameraMatrices.h"
#include "SimpleDrawNode.h"

const unsigned int sprintCount{ 9 };
std::vector<std::pair<glm::vec3, glm::vec3>> fireQuads;

float offset1{ 0.51f };
float offset2{ -0.009f };//weird vertical adjustments

float denom{ 6.08f };
float num{ 0.03f };//weird horizontal adjustments

const glm::vec2 UV_SCALE{ glm::vec2(1. / 6., .48) };

const glm::vec2 UV_OFFSETS[sprintCount]
{
	/*glm::vec2(0, 0),*/glm::vec2((1. + num) / denom, offset2),glm::vec2((2. + num) / denom, offset2), glm::vec2((3. + num) / denom, offset2),
	glm::vec2((4. + num) / denom, offset2), glm::vec2((5. + num) / denom, offset2), /*glm::vec2(6. / 6, 0),*/
	 glm::vec2((1. + num) / denom, offset1), glm::vec2((2. + num) / denom, offset1), glm::vec2((3. + num + 0.04) / denom, offset1),
	glm::vec2((4. + num + 0.04) / denom, offset1), /*glm::vec2(5. / 6, .5)*/
};//commented out the ones that either don't have an image or have no fire.



const float MAX_LIFE{ .1f };//time between animation changes.


void ofApp::setup()
{
	using namespace glm;
	ofHideCursor();
	ofDisableArbTex();
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	ofSetFrameRate(0);
	ofSetVerticalSync(false);

	auto window{ ofGetCurrentWindow() };
	reloadShaders();

	cubeMesh.load("models/cube.ply");

	sceneGraphRoot.childNodes.emplace_back(new SceneGraphNode{});

	CubeNode = sceneGraphRoot.childNodes.back();

	CubeNode->childNodes.emplace_back(new SimpleDrawNode{ cubeMesh, shader });
	CubeNode->childNodes.back()
		->localTransform = translate(vec3(0, -1, 0)) * scale(vec3(100, 0.02, 100));//ground plane

	CubeNode->childNodes.emplace_back(new SimpleDrawNode{ cubeMesh, shader });
	CubeNode->childNodes.back()
		->localTransform = translate(vec3(0, .2, -2));//reference cube

	CubeNode->childNodes.emplace_back(new SimpleDrawNode{ cubeMesh, shader });
	CubeNode->childNodes.back()
		->localTransform = translate(vec3(0, 20, -2));//sky cube

	fireQuad.addVertex(vec3(-1, -1, 0));
	fireQuad.addVertex(vec3(-1, 1, 0));
	fireQuad.addVertex(vec3(1, 1, 0));
	fireQuad.addVertex(vec3(1, -1, 0));

	fireQuad.addTexCoord(vec2(0, 0));
	fireQuad.addTexCoord(vec2(0, 1));
	fireQuad.addTexCoord(vec2(1, 1));
	fireQuad.addTexCoord(vec2(1, 0));

	ofIndexType indices[6] = { 0, 1, 2, 2, 3, 0 };
	fireQuad.addIndices(indices, 6);

	smokeQuad.addVertex(vec3(-1, -1, 0));
	smokeQuad.addVertex(vec3(-1, 1, 0));
	smokeQuad.addVertex(vec3(1, 1, 0));
	smokeQuad.addVertex(vec3(1, -1, 0));

	smokeQuad.addTexCoord(vec2(0, 0));
	smokeQuad.addTexCoord(vec2(0, 1));
	smokeQuad.addTexCoord(vec2(1, 1));
	smokeQuad.addTexCoord(vec2(1, 0));

	smokeQuad.addIndices(indices, 6);

	smokeImg.load("textures/smoke_07.png");
	fireImg.load("textures/campfire-sprites-without-bg.png");
	fireImg.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
}

void ofApp::reloadShaders()
{
	shader.load("shaders/my.vert", "shaders/my.frag");
	fireShader.load("shaders/my.vert", "shaders/textureFire.frag");
	smokeShader.load("shaders/my.vert", "shaders/texture.frag");
	needsReload = false;
}

void ofApp::updateCameraRotation(float dx, float dy)
{
	using namespace glm;

	cameraPitch -= dy;
	cameraPitch = glm::clamp(cameraPitch, radians(-89.0f), radians(89.0f));

	cameraHead -= dx;
}

void ofApp::update()
{
	if (needsReload)
	{
		needsReload = false;
		reloadShaders();
	}

	float dt{ static_cast<float>(ofGetLastFrameTime()) };

	using namespace glm;
	if (prevX != 0 && prevY != 0)//little optimization that really didn't help
	{
		camera.position += mat3(rotate(cameraHead, vec3(0, 1, 0)))
			* velocity * dt;
		camera.rotation = rotate(cameraHead, vec3(0, 1, 0)) *
			rotate(cameraPitch, vec3(1, 0, 0));
	}

	particleSystem.update(ofGetLastFrameTime());

	timeSinceRegen += dt;

	if (timeSinceRegen > MAX_LIFE)
	{

		timeSinceRegen = 0;
		blendParam = 0.0f;
		currentPng++;
		currentPng = (currentPng % sprintCount);
	}

	if (timeSinceRegen > 0.5f * MAX_LIFE)
	{

		blendParam += 10.0f * dt;

		if (blendParam > 1.0f)
		{
			blendParam = 1.0f;
		}
	}


}

glm::mat4 screenAlignedBillboard(glm::mat4 modelView)
{
	using namespace glm;
	vec3 camSpacePos{ modelView * vec4(0, 0, 0, 1) };
	return translate(camSpacePos);
}

void ofApp::draw()
{
	using namespace glm;
	int CulledSystems = 0;
	float width{ static_cast<float>(ofGetViewportWidth()) };
	float height{ static_cast<float>(ofGetViewportHeight()) };
	float aspect = width / height;

	CameraMatrices camMatrices{ camera, aspect, 0.01f, 200.0f, };

	shader.begin();
	shader.setUniform3f("lightDir", normalize(vec3(1, .5, .9)));
	shader.setUniform3f("lightColor", vec3(1, 1, 1)); // white light  
	shader.setUniform3f("meshColor", vec3(.6, .6, .6)); // white material  
	shader.setUniform3f("ambientColor", vec3(.1));
	shader.end();
	ofEnableDepthTest();//enable depth test for the ground plane and cubes
	sceneGraphRoot.drawSceneGraph(camMatrices);
	float gridOffset = (xbyx - 1) * 15.0f / 2.0f;//calculate the offset to make the grid centered on the scene.
	ofDisableDepthTest();//disable depth test for the fire and smoke
	fireQuads.clear();
	for (int i = 0; i < xbyx; i++)
	{
		for (int j = 0; j < xbyx; j++)
		{

			float x = i * 15.0f - gridOffset;//Each particle system is 15 units apart
			float z = j * 15.0f - gridOffset;

			glm::vec3 toParticleSystem = vec3(x, 0, z) - camera.position;//difference between particlesystem position and camera position
			glm::vec3 viewSpacePos = glm::transpose(camera.rotation) * toParticleSystem;//determine the position of the particlesystem relative to the camera
			fireQuads.emplace_back(std::make_pair(glm::vec3(toParticleSystem), glm::vec3(x, 0, z)));//store the position of the particlesystem
		}
	}
	std::sort(fireQuads.begin(), fireQuads.end(), [](const auto& a, const auto& b) { return glm::length(a.first) > glm::length(b.first); });//sort the particlesystems by length to camera so that we can render them in the right order
	for (const auto& afireQuad : fireQuads) {

		float x = afireQuad.second.x;
		float z = afireQuad.second.z;
		float dx = x - camera.position.x;
		float dz = z - camera.position.z;

		glm::vec3 toParticleSystem = vec3(x, 0, z) - camera.position;//difference between particlesystem position and camera position
		glm::vec3 viewSpacePos = glm::transpose(camera.rotation) * toParticleSystem;//determine the position of the particlesystem relative to the camera
		bool isCloseToCamera = glm::length(toParticleSystem) < 20.0f;

		if (FullFrustumCulling) {
			//FOV not directly pulled from camera because our objects have width and height beyond their position
			//so I manually tweaked these until it was as low as I could without noticing anything being removed.

			float mod = isCloseToCamera ? 30.0f : 0.0f;//Close up objects appear larger and therefore have more width and height beyond their position

			//I guess that makes it not technically a Frustum, because the camera end is weirdly bigger. It's almost like an arrow with the end chopped off?

			float tanHalfHFov = tan(glm::radians((125.0f + mod) / 2.0f));   // Calculate the tangent of half the horizontal field of view (FOV) in radians.  
			float tanHalfVFov = tan(glm::radians((115.0f + mod) / 2.0f));   // Calculate the tangent of half the vertical field of view (FOV) in radians. 

			// Check if the particle system is within the horizontal FOV.    
			bool inHorizontalFOV = abs(viewSpacePos.x) < -viewSpacePos.z * tanHalfHFov;

			// Check if the particle system is within the vertical FOV.  
			bool inVerticalFOV = abs(viewSpacePos.y) < -viewSpacePos.z * tanHalfVFov;

			if (!(inHorizontalFOV && inVerticalFOV && viewSpacePos.z > -200.0f && viewSpacePos.z < -0.01f)) {//check if the particle system is too far away or too close to the camera
				CulledSystems++;
				continue;
			}
		}
		else if (FrustumCulling2D) {
			float dx = x - camera.position.x;
			float dz = z - camera.position.z;

			//FOV not directly pulled from camera because our objects have width and height beyond their position
			//so I manually tweaked these until it was as low as I could without noticing anything being removed.

			float mod = isCloseToCamera ? 30.0f : 0.0f;//Close up objects appear larger and therefore have more width and height beyond their position

			float tanHalfFov = tan(glm::radians((125.0f + mod) / 2.0f)); // Calculate the tangent of half the horizontal field of view (FOV) in radians.  


			if (abs(viewSpacePos.x) > -viewSpacePos.z * tanHalfFov) {// Check if the particle system is within the horizontal FOV.
				CulledSystems++;
				continue;
			}
		}
		else if (BehindCameraCulling) {
			if (viewSpacePos.z >= 0.0f) {//check if the particle system is behind the camera.
				CulledSystems++;
				continue;
			}

		}



		mat4 mvpFire =
			camMatrices.getProj() * screenAlignedBillboard(camMatrices.getView() * translate(vec3(x, 0, z)));

		fireShader.begin();
		fireShader.setUniformMatrix4f("mvp", mvpFire);
		fireShader.setUniformTexture("tex", fireImg, 0);
		fireShader.setUniform2f("uvScale", UV_SCALE);

		// Where in the texture to start for the current fire
		fireShader.setUniform2f("uvStart", UV_OFFSETS[currentPng]);

		// Where in the texture to start for the next face
		fireShader.setUniform2f("uvStart2", UV_OFFSETS[(currentPng + 1) % sprintCount]);
		fireShader.setUniform1f("blendParam", blendParam);
		fireQuad.draw();
		fireShader.end();

		particleSystem.begin();

		for (BasicParticle& particle : particleSystem)
		{
			smokeShader.begin();
			float life = 9.5 * particle.getLife() * pow(1 - particle.getLife(), 3);//actually graphed this on desmos to find a curve I liked

			smokeShader.setUniformTexture("tex", smokeImg, 0);
			smokeShader.setUniformMatrix4f("mvp", mvpFire);

			smokeShader.setUniform2f("uvScale", vec2(1));//multiply by 1 so I can ignore the special parts of the vert that are for the fire animation.

			smokeShader.setUniform1f("opacityMod", life);
			smokeShader.setUniform3f("translation", particle.getXPos(), particle.getYPos()+1, 0);
			smokeQuad.draw();
			smokeShader.end();
		}
		particleSystem.end();



	}

	string framerateText = "Framerate: " + ofToString(ofGetFrameRate(), 2) + " FPS";
	ofSetColor(255, 255, 255);
	ofDrawBitmapString(framerateText, 10, 20);

	string cullingText = "[1] - Behind Camera Culling: " + string(BehindCameraCulling ? "Enabled" : "Disabled");
	ofDrawBitmapString(cullingText, 10, 40);

	string frustumText = "[2] - 2D Frustum Culling: " + string(FrustumCulling2D ? "Enabled" : "Disabled");
	ofDrawBitmapString(frustumText, 10, 60);

	string fullFrustumText = "[3] - Full Frustum Culling: " + string(FullFrustumCulling ? "Enabled" : "Disabled");
	ofDrawBitmapString(fullFrustumText, 10, 80);

	string totalParticleSystems = "Total Particle Systems: " + ofToString(xbyx * xbyx);
	ofDrawBitmapString(totalParticleSystems, 10, 100);

	string culledText = "Particle System Culling: " + ofToString(CulledSystems);
	ofDrawBitmapString(culledText, 10, 120);

	string addingText = "Press SHift and any number to increase the # of Sprites";
	ofDrawBitmapString(addingText, 10, 140);
}




//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	using namespace glm;

	float moveSpeed = 1.0f;
	if (key == OF_KEY_UP || key == 'w')
	{
		velocity.z = -moveSpeed;
	}
	else if (key == OF_KEY_DOWN || key == 's')
	{
		velocity.z = moveSpeed;
	}
	else if (key == OF_KEY_LEFT || key == 'a')
	{
		velocity.x = -moveSpeed;
	}
	else if (key == OF_KEY_RIGHT || key == 'd')
	{
		velocity.x = moveSpeed;
	}
	if (key == '1') {//toggles for culling methods, turn off the others when one is on
		BehindCameraCulling = !BehindCameraCulling;
		FrustumCulling2D = false;
		FullFrustumCulling = false;
		culled = false;
	}
	if (key == '2') {
		FrustumCulling2D = !FrustumCulling2D;
		BehindCameraCulling = false;
		FullFrustumCulling = false;
		culled = false;
	}
	if (key == '3') {
		FullFrustumCulling = !FullFrustumCulling;
		BehindCameraCulling = false;
		FrustumCulling2D = false;
		culled = false;
	}
	if (key == '!') {//lets you adjust the grid size live, shift+number of particles in a row/column
		xbyx = 1;
	}
	if (key == '@') {
		xbyx = 2;
	}
	if (key == '#') {
		xbyx = 3;
	}
	if (key == '$') {
		xbyx = 4;
	}
	if (key == '%') {
		xbyx = 5;
	}
	if (key == '^') {
		xbyx = 6;
	}
	if (key == '&') {
		xbyx = 7;
	}
	if (key == '*') {
		xbyx = 8;
	}
	if (key == '(') {
		xbyx = 9;
	}
	if (key == ')') {
		xbyx = 10;
	}
	if (key == '+') {
		xbyx += 5;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	if (key == '`')
	{
		// Reload shaders
		needsReload = true;
	}
	else if (key == OF_KEY_LEFT || key == 'a' || key == OF_KEY_RIGHT || key == 'd')
	{
		// Reset velocity when a key is released
		velocity.x = 0;
	}
	else if (key == OF_KEY_UP || key == 'w' || key == OF_KEY_DOWN || key == 's')
	{
		// Reset velocity when a key is released
		velocity.z = 0;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
	if (prevX != 0 && prevY != 0)
	{
		// Previous mouse position has been initialized.
		// Calculate dx and dy
		int dx = x - prevX;
		int dy = y - prevY;

		updateCameraRotation(mouseSensitivity * dx, mouseSensitivity * dy);
	}

	prevX = x;
	prevY = y;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	if (prevX != 0 && prevY != 0)
	{
		// Previous mouse position has been initialized.
		// Calculate dx and dy
		int dx = x - prevX;
		int dy = y - prevY;

	}

	prevX = x;
	prevY = y;
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

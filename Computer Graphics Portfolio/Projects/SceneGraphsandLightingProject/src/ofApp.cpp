#include "ofApp.h"
using namespace glm;
//--------------------------------------------------------------
void ofApp::setup()
{
	// IMPORTANT!  Don't remove this line or textures won't work:
	ofDisableArbTex();
	//ofEnableAlphaBlending();
	ofEnableDepthTest();
	glEnable(GL_CULL_FACE);
	sceneGraph.setup(shader);
}

void ofApp::reloadShaders()
{
	// Load shaders here:
	shader.load("shaders/my.vert", "shaders/my.frag");
}

void ofApp::updateCameraRotation(float dx, float dy)
{
	using namespace glm;
	cameraHead += dx;
	cameraPitch += dy;
	cameraPitch = glm::clamp(cameraPitch, -static_cast<float>(PI) / 2, static_cast<float>(PI) / 2);
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
	camera.position += velocityWorldSpace * dt;
	camera.rotation = rotate(-cameraHead, vec3(0, 1, 0)) * rotate(-cameraPitch, vec3(1, 0, 0));

	sceneGraph.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw()
{
	using namespace glm;

	float width{ static_cast<float>(ofGetViewportWidth()) };
	float height{ static_cast<float>(ofGetViewportHeight()) };
	float aspect{ width / height };
	
	/*
	float farPlane{ 800.0 };
	mat4 proj{ perspective(radians(90.0f), aspect, 0.01f, 2000.0f) };*/

	CameraMatrices camMatrices{ camera, aspect,0.01, 100.0f };
	shader.setUniform3f("meshColor", vec3(1.0f, 1.0f, 1.0f));
	shader.setUniform3f("lightDirection", vec3(0.0f, 0.0f, 1.0f));
	shader.setUniform3f("lightColor", vec3(1.0f, 1.0f, 1.0f));
	sceneGraph.draw(camMatrices);

}

//--------------------------------------------------------------
void ofApp::exit()
{
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{

	float moveSense = 5.0f;
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


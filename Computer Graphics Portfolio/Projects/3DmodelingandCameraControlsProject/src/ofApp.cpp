#include "ofApp.h"
#include <GLFW/glfw3.h>
#include "buildTerrainMesh.h"

using namespace glm;
//--------------------------------------------------------------
void ofApp::setup()
{
	// IMPORTANT!  Don't remove this line or textures won't work:
	ofDisableArbTex();
	ofEnableDepthTest();
	glEnable(GL_CULL_FACE);

	auto window{ ofGetCurrentWindow() };
	glfwSetInputMode(dynamic_pointer_cast<ofAppGLFWWindow>(window)->getGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	window->setFullscreen(true);
	sphere.load("models/sphere.ply");
	torus.load("models/torus.ply");
	block.load("models/block.ply");
	mesh.load("models/Project2.ply");//torus+block+sphere
	shader.load("shaders/my.vert", "shaders/my.frag");
	torus.flatNormals();
	sphere.flatNormals();
	meshPosition = vec3(ADMove, QEMove, WSMove);

	myMesh = mesh;//set what you want to make here

	myVBO.setMesh(myMesh, GL_STATIC_DRAW);
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
}
//--------------------------------------------------------------
void ofApp::update()
{
	// First, check for shader reload via hotkey:
	if (needsReload)
	{
		reloadShaders();
		needsReload = false;
		//using namespace glm;
	}

	//// calculate world space velocity
	vec3 velocityWorldSpace{ mat3( rotate(-cameraHead, vec3(0, 1, 0))*rotate(-cameraPitch,vec3(1,0,0))) * velocity };

	//// time since last frame
	float dt{ static_cast<float>(ofGetLastFrameTime()) };

	//// update position
	position += velocityWorldSpace * dt;

	meshPosition = { ADMove, QEMove, WSMove };
}
//--------------------------------------------------------------
void ofApp::draw()//Very choppy before vbo, with the vbo it's still a little choppy but not as bad. (2000 models)
{
	//for(int i=0;i<2000;i++){
	shader.begin();
	float fov = radians(105.0f);
	float aspect = (float)ofGetViewportWidth() / (float)ofGetViewportHeight();
	float Near = 0.1f;
	float Far = 10.0f;
	
	mat4 model{ rotate(cameraPitch, vec3(1,0,0)) * rotate(cameraHead, vec3(0,1,0)) * translate(position) };
	mat4 view{};
	mat4 projection{ perspective(fov,aspect,Near,Far) };
	mat4 mvp{ projection * view * model };
	shader.setUniformMatrix4f("model", model);
	shader.setUniformMatrix4f("mvp", mvp);
	shader.setUniformMatrix4f("MV", view * model);
	myVBO.drawElements(GL_TRIANGLES, myVBO.getNumIndices());
	//myMesh.draw();
	shader.end();

	vec3 CamPosition = vec3((view * model)[3]);
	cout << distance(CamPosition, vec3(0, 0, 0)) << endl;

	//}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if (key == 'w')
	{
		velocity.z = 1;
	}
	else if (key == 's')
	{
		velocity.z = -1;
	}
	else if (key == 'a')
	{
		velocity.x = 1;
	}
	else if (key == 'd')
	{
		velocity.x = -1;
	}
	else if (key == 'e')
	{
		velocity.y = -1;
	}
	else if (key == 'q')
	{
		velocity.y = 1;
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
	else if (key == 'q' || key == 'e') {
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

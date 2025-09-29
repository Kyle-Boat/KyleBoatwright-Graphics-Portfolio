#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // IMPORTANT!  Don't remove this line or textures won't work:
    ofDisableArbTex();
    using namespace glm;
    vec2 texSize;
        texSize = vec2(4, 1.5);
	int texSize2 = 5;
    quad.addVertex(vec3(-1, 1, 0) );
    quad.addTexCoord(vec2(0, 0)*texSize);

    quad.addVertex(vec3(-1, -1, 0) );
    quad.addTexCoord(vec2(0, 1)*texSize);

    quad.addVertex(vec3(1, -1, 0) );
    quad.addTexCoord(vec2(1, 1) * texSize);


    quad.addVertex(vec3(1, 1, 0) );
    quad.addTexCoord(vec2(1, 0) * texSize);


    quad2.addVertex(vec3(-1, 1, 0));
    quad2.addTexCoord(vec2(0, 0) * texSize2);

    quad2.addVertex(vec3(-1, -1, 0));
    quad2.addTexCoord(vec2(0, 1) * texSize2);

    quad2.addVertex(vec3(1, -1, 0));
    quad2.addTexCoord(vec2(1, 1) * texSize2);


    quad2.addVertex(vec3(1, 1, 0));
    quad2.addTexCoord(vec2(1, 0) * texSize2);
    ofIndexType indices[6] = { 0,1,2,2,3,0 };

    quad.addIndices(indices, 6);
	quad2.addIndices(indices, 6);

    shader.load("shaders/my.vert", "shaders/my.frag");
    img.load("textures/spark_05.png");
	img3.load("textures/spark_06.png");
    shader2.load("shaders/my.vert", "shaders/my2.frag");
	img2.load("textures/smoke_04.png");
}

void ofApp::reloadShaders()
{
    // Load shaders here:

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

    // Add additional update logic as needed:
	particleSystem.update(ofGetLastFrameTime());
    particleSystem2.update(ofGetLastFrameTime());
}

//--------------------------------------------------------------
void ofApp::draw()
{
    //shader.begin
    // //shader.setUniform3f("position",particle.getPoisition()):
   // quad.dtaw
    //shader.end()
    particleSystem.begin();
	particleSystem2.begin();
    for (BasicParticle& particle : particleSystem) {
        /*particle.getColor();
        particle.getPosition*/
        //or something like that can happen here
        if (particle.getType()%2==0) {
            shader.setUniformTexture("tex", img, 0);
		}
		else {
			shader.setUniformTexture("tex", img3, 0);
		}
        shader.begin();
		float fragMod = pow(1-(abs(particle.getLife()-0.5)*2),10);
        shader.setUniform1f("fragMod", fragMod);
        shader.setUniform3f("translation", particle.getXPos(), particle.getYPos()-.23, 0.5);
        quad.draw();
        shader.end();
    }
	for (const BasicParticle& particle : particleSystem2)
	{
        shader2.setUniformTexture("tex", img2, 0);
		shader2.begin();
        float fragMod = 1 - (abs(particle.getLife() - 0.5) * 2);
		shader2.setUniform1f("fragMod", fragMod);
		shader2.setUniform3f("translation", particle.getXPos()+.15, particle.getYPos()+.1, 0.5);
		quad2.draw();
		shader2.end();
	}
    particleSystem.end();
    particleSystem2.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    // Shader reload hotkey
    if (key == '`')
    {
        needsReload = true;
    }

    // Handle other key press events as needed:

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

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

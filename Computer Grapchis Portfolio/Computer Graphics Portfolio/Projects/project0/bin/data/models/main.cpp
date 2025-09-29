#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{
	ofGLWindowSettings glSettings;
	glSettings.setSize(1024, 768);
	glSettings.windowMode = OF_WINDOW;
	glSettings.setGLVersion(4, 1);
	auto window { ofCreateWindow(glSettings) };
	//window->setFullscreen(true);

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());
}

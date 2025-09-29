#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{
    ofGLWindowSettings glSettings;
    glSettings.setSize(1024, 768);
    glSettings.windowMode = OF_WINDOW;
    glSettings.setGLVersion(4, 1);
    ofCreateWindow(glSettings);

    printf("%s\n", glGetString(GL_VERSION));
    ofRunApp(new ofApp());
}

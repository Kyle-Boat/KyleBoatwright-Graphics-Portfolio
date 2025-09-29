#include "ofApp.h"
#include "CameraMatrices.h"
using namespace glm;
#include "calcTangents.h"

void ofApp::drawCube(const CameraMatrices& camMatrices, ofShader& skyboxShader, ofMesh& cubeMesh) {
    
    skyboxShader.begin();
    glDepthFunc(GL_LEQUAL);
    skyboxShader.setUniformMatrix4f("mvp", camMatrices.getProj() * mat4(mat3(camMatrices.getView())));//converts back and forth to clip off the translation so you can't escape skybox
    skyboxShader.setUniformTexture("cubemap", cubeMap.getTexture(),0);
    cubeMesh.draw();
    skyboxShader.end();
    glDepthFunc(GL_LESS);
}
void buildCube(ofMesh& mesh)
{
    using namespace glm;

    // Generate the cube mesh
    mesh.addVertex(vec3(-1, -1, -1)); // front left bottom
    mesh.addVertex(vec3(-1, -1, 1)); // back  left bottom
    mesh.addVertex(vec3(-1, 1, -1)); // front left top
    mesh.addVertex(vec3(-1, 1, 1));  // back  left top
    mesh.addVertex(vec3(1, -1, -1)); // front right bottom
    mesh.addVertex(vec3(1, -1, 1));  // back  right bottom
    mesh.addVertex(vec3(1, 1, -1));  // front right top
    mesh.addVertex(vec3(1, 1, 1));   // back  right top

    ofIndexType indices[36] =
    {
        2, 3, 6, 7, 6, 3, // top
        0, 4, 1, 1, 4, 5, // bottom
        0, 1, 2, 2, 1, 3, // left
        4, 6, 5, 5, 6, 7, // right
        0, 2, 4, 4, 2, 6, // front
        1, 5, 3, 3, 5, 7, // back
    };

    mesh.addIndices(indices, 36);

    mesh.flatNormals();

    for (size_t i{ 0 }; i < mesh.getNumNormals(); i++)
    {
        mesh.setNormal(i, -mesh.getNormal(i));
    }
}

void buildCircle(ofMesh& mesh, int subdiv)
{
    using namespace glm;

    // Populate with vertex positions.

    // Center
    mesh.addVertex(vec3(0));

    // Outer ring
    for (int i{ 0 }; i < subdiv; i++)
    {
        double theta{ i * (2 * PI) / subdiv };
        mesh.addVertex(vec3(cos(theta), sin(theta), 0));
    }

    for (int i{ 0 }; i < subdiv; i++)
    {
        mesh.addIndex(0);
        mesh.addIndex(1 /* outer ring start */ + i);
        mesh.addIndex(1 /* outer ring start */ + (i + 1) % subdiv);
    }

    mesh.flatNormals();

    for (size_t i{ 0 }; i < mesh.getNumNormals(); i++)
    {
        mesh.setNormal(i, -mesh.getNormal(i));
    }
}

void buildCylinder(ofMesh& mesh, int subdiv)
{
    using namespace glm;

    // Populate with vertex positions.

    // TOP
    // Top center
    mesh.addVertex(vec3(0, 1, 0)); // vertex 0
    mesh.addTexCoord(vec2(0.5, 0.5) / PI);
    mesh.addNormal(vec3(0, 1, 0));

    // Top outer ring -- vertices 1 through subdiv
    for (int i{ 0 }; i < subdiv; i++)
    {
        double theta{ i * (2 * PI) / subdiv };
        mesh.addVertex(vec3(cos(theta), 1, -sin(theta)));
        mesh.addTexCoord(vec2(cos(theta) * 0.5 + 0.5, sin(theta) * 0.5 + 0.5) / PI);
        mesh.addNormal(vec3(0, 1, 0));
    }

    // BOTTOM
    // Bottom center
    mesh.addVertex(vec3(0, -1, 0)); // vertex subdiv + 1
    mesh.addTexCoord(vec2(0.5, 0.5) / PI);
    mesh.addNormal(vec3(0, -1, 0));

    // Bottom outer ring -- vertices subdiv + 2 through 2 * subdiv + 1
    for (int i{ 0 }; i < subdiv; i++)
    {
        double theta{ i * (2 * PI) / subdiv };
        mesh.addVertex(vec3(cos(theta), -1, -sin(theta)));
        mesh.addTexCoord(vec2(cos(theta) * 0.5 + 0.5, sin(theta) * 0.5 + 0.5) / PI);
        mesh.addNormal(vec3(0, -1, 0));
    }

    // SIDES
    // Top outer ring -- vertices 2 * subdiv + 2 through 3 * subdiv + 2
    for (int i{ 0 }; i < subdiv; i++)
    {
        double u{ static_cast<double>(i) / subdiv };
        double theta{ u * (2 * PI) };

        mesh.addVertex(vec3(cos(theta), 1, -sin(theta)));
        mesh.addTexCoord(vec2(u, 1 / PI));
        mesh.addNormal(vec3(cos(theta), 0, -sin(theta)));
    }

    // Bottom outer ring -- vertices 3 * subdiv + 2 through 4 * subdiv + 1
    for (int i{ 0 }; i < subdiv; i++)
    {
        double u{ static_cast<double>(i) / subdiv };
        double theta{ u * (2 * PI) };

        mesh.addVertex(vec3(cos(theta), -1, -sin(theta)));
        mesh.addTexCoord(vec2(u, 0));
        mesh.addNormal(vec3(cos(theta), 0, -sin(theta)));
    }

    // Start of index buffer: populate to form faces

    // Top circle
    for (int i{ 0 }; i < subdiv; i++)
    {
        int topRingStart{ 1 };
        mesh.addIndex(0); // top center
        mesh.addIndex(topRingStart + i);
        mesh.addIndex(topRingStart + (i + 1) % subdiv);
    }

    // Bottom circle
    for (int i{ 0 }; i < subdiv; i++)
    {
        int bottomRingStart{ subdiv + 2 };
        mesh.addIndex(subdiv + 1); // bottom center

        // Swap order from top circle so that the circle faces downwards instead of upwards.
        mesh.addIndex(bottomRingStart + (i + 1) % subdiv);
        mesh.addIndex(bottomRingStart + i);
    }

    // Sides
    for (int i{ 0 }; i < subdiv; i++)
    {
        int topRingStart{ 2 * subdiv + 2 };
        int bottomRingStart{ 3 * subdiv + 2 };

        // Define the indices of a single quad
        int indices[4]
        {
            topRingStart + i,
            topRingStart + (i + 1) % subdiv,
            bottomRingStart + i,
            bottomRingStart + (i + 1) % subdiv
        };

        // build the quad
        mesh.addIndex(indices[0]);
        mesh.addIndex(indices[2]);
        mesh.addIndex(indices[1]);
        mesh.addIndex(indices[1]);
        mesh.addIndex(indices[2]);
        mesh.addIndex(indices[3]);
    }

    /*   mesh.flatNormals();

       for (size_t i { 0 }; i < mesh.getNumNormals(); i++)
       {
           mesh.setNormal(i, -mesh.getNormal(i));
       }*/
}

void buildSphere(ofMesh& mesh, int subdivTheta, int subdivPhi)
{
    using namespace glm;

    // Populate with vertex positions.

    // Rings
    for (int i{ 0 }; i <= subdivTheta; i++)
    {
        double v{ static_cast<double>(i) / subdivTheta };
        double theta{ v * PI };

        for (int j{ 0 }; j <= subdivPhi; j++)
        {
            double u{ static_cast<double>(j) / subdivPhi };
            double phi{ u * (2 * PI) };

            mesh.addVertex(vec3(sin(theta) * cos(phi), cos(theta), -sin(theta) * sin(phi)));
            mesh.addNormal(vec3(sin(theta) * cos(phi), cos(theta), -sin(theta) * sin(phi)));
            mesh.addTexCoord(vec2(u, v));
        }
    }

    // Start of index buffer: populate to form faces

    // Top circle (around north pole)
    for (int i{ 0 }; i < subdivPhi; i++)
    {
        int topRingStart{ (subdivPhi + 1) };

        mesh.addIndex(i); // top center / north pole
        mesh.addIndex(topRingStart + i);
        mesh.addIndex(topRingStart + (i + 1));
    }

    int southPoleStart{ subdivTheta * (subdivPhi + 1) };

    // Bottom circle (around south pole)
    for (int i{ 0 }; i < subdivPhi; i++)
    {
        int bottomRingStart{ southPoleStart - (subdivPhi + 1) };

        mesh.addIndex(southPoleStart + i); // bottom center / south pole

        // Swap order from top circle so that the circle faces downwards instead of upwards.
        mesh.addIndex(bottomRingStart + (i + 1));
        mesh.addIndex(bottomRingStart + i);
    }

    // Sides
    for (int i{ 0 }; i < subdivTheta - 2; i++)
    {
        for (int j{ 0 }; j < subdivPhi; j++)
        {
            int topRingStart{ (subdivPhi + 1) * (i + 1) };
            int bottomRingStart{ (subdivPhi + 1) * (i + 2) };

            // Define the indices of a single quad
            int indices[4]
            {
                topRingStart + j,
                topRingStart + (j + 1),
                bottomRingStart + j,
                bottomRingStart + (j + 1)
            };

            // build the quad
            mesh.addIndex(indices[0]);
            mesh.addIndex(indices[2]);
            mesh.addIndex(indices[1]);
            mesh.addIndex(indices[1]);
            mesh.addIndex(indices[2]);
            mesh.addIndex(indices[3]);
        }
    }

    //mesh.flatNormals();

    //for (size_t i { 0 }; i < mesh.getNumNormals(); i++)
    //{
    //    mesh.setNormal(i, -mesh.getNormal(i));
    //}
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofDisableArbTex(); // IMPORTANT!

    ofEnableDepthTest();

    //glEnable(GL_CULL_FACE); // Enable face culling to test winding order
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 
    // Load shaders for the first time
    reloadShaders();

    // Load torus mesh
    torusMesh.load("models/cube.ply");
    hammerMesh.load("models/Mjolnir.ply");
    //hammerMesh.flatNormals();
    calcTangents(hammerMesh);
    // Load shield textures
    hammerDiffuse.load("textures/Mjolnir_Diffuse.png");
    hammerNormal.load("textures/Mjolnir_Normal.png");
	hammerSpecular.load("textures/Mjolnir_Specular.png");
    // Generate cube mesh
    buildCube(cubeMesh);
	hammerDiffuse.getTexture().generateMipmap(); // create the mipmaps
	hammerDiffuse.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Generate circle mesh
    buildCircle(circleMesh, 40);

    // Generate cylinder mesh
    buildCylinder(cylinderMesh, 8);

    buildSphere(sphereMesh, 8, 16);
    cubeMesh.load("models/cube.ply");
    // load world texture
    worldTex.load("textures/aerial_grass_rock_diff_4k.png");
    cubeMap.load("textures/skybox_front.png", "textures/skybox_back.png",  "textures/skybox_right.png", "textures/skybox_left.png", "textures/skybox_top.png", "textures/skybox_bottom.png");
    cubeMap.getTexture().generateMipmap();

    // load metal texture
    metalTex.load("textures/water_nrm.png");
    metalTex.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
    metalTex.getTexture().generateMipmap(); // create the mipmaps
    metalTex.getTexture().setTextureMinMagFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

void ofApp::reloadShaders()
{
    shader.load("shaders/my.vert", "shaders/my.frag");
	skyboxShader.load("shaders/skybox.vert", "shaders/skybox.frag");
    needsReload = false;
}

void ofApp::updateCameraRotation(float dx, float dy)
{
    cameraHead += dx;
    cameraPitch += dy;

    using namespace glm;
    cameraPitch = glm::clamp(cameraPitch, -static_cast<float>(PI) / 2, static_cast<float>(PI) / 2);
}
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

	position += velocityWorldSpace * dt;
    // update position
    camera.position += velocityWorldSpace * dt;
    camera.rotation = rotate(-cameraHead, vec3(0, 1, 0)) * rotate(-cameraPitch, vec3(1, 0, 0));

}


//--------------------------------------------------------------
void ofApp::draw( )
{
    using namespace glm;

    float width{ static_cast<float>(ofGetViewportWidth()) };
    float height{ static_cast<float>(ofGetViewportHeight()) };
    float aspect{ 1024.0f / 768.0f };

    mat4 model{ rotate(radians(45.0f), vec3(1, 1, 1)) * scale(vec3(0.5, 0.5, 0.5)) };
    mat4 view{ rotate(cameraPitch, vec3(1, 0, 0)) * rotate(cameraHead, vec3(0, 1, 0))
        * translate(-position) };
    mat4 proj{ perspective(radians(90.0f), aspect, 0.01f, 10.0f) };
    mat4 mvp{ proj * view * model };
    CameraMatrices camMatrices{ camera, aspect, 0.01, 100.0f };
    drawCube(camMatrices, skyboxShader, cubeMesh);
    shader.begin(); // start using the shader
    shader.setUniformMatrix3f("normalMatrix", mat3(model));
	shader.setUniform3f("CamPosition", camMatrices.getCamera().position);
    shader.setUniformMatrix4f("mvp", mvp);
    shader.setUniformMatrix4f("model", model);
    //shader.setUniform3f("meshColor", vec3(1, 0.1f, 0.1f)); // red surface
    shader.setUniform3f("lightColor", vec3(1)); // white light
    shader.setUniform3f("lightDir", normalize(vec3(-1, 1, 1)));
	shader.setUniform3f("pointLightPos", vec3(0,-.1,-.1));
	shader.setUniform3f("pointLightColor", vec3(.1, .1f, 1));
	shader.setUniformTexture("envMap", cubeMap.getTexture(), 3);
    shader.setUniformTexture("diffuseTex", hammerDiffuse, 0);
    shader.setUniformTexture("normalTex", hammerNormal, 1);
	shader.setUniformTexture("specularTex", hammerSpecular, 2);
    //torusMesh.draw();
    hammerMesh.draw();
    shader.end(); // done with the shader
   
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == '`')
    {
        reloadShaders();
    }
    else if (key == 'w')
    {
        velocity.z = -1;
    }
    else if (key == 's')
    {
        velocity.z = 1;
    }
    else if (key == 'a')
    {
        velocity.x = -1;
    }
    else if (key == 'd')
    {
        velocity.x = 1;
    }
    else if (key == 'e')
    {
        velocity.y = 1;
    }
    else if (key == 'q')
    {
        velocity.y = -1;
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


#include "MySceneGraph.h"
#include "SimpleDrawNode.h"
#include "CustomAnimationNode.h"
#include "LitDrawNode.h"
#include "SpotLightNode.h"
#include "PointLight.h"
#include "SimpleAnimationNode.cpp"
void MySceneGraph::setup(ofShader& shader)
{
    using namespace glm;


    lighting.dirLight.direction = vec3(0.25, -1, -.5);
    lighting.dirLight.color = vec3(.9, .9, .75);
    lighting.dirLight.intensity = 0.2;
    lighting.ambientLight = vec3(.1, .1, 0.25);

    // Load cube mesh
    cubeMesh.load("models/cube.ply");
	sphereMesh.load("models/sphere.ply");
    torusMesh.load("models/torus.ply");
    
    // create cube node
    std::shared_ptr<LitDrawNode> cubeNode{
        new LitDrawNode { cubeMesh, shader , lighting, glm::vec3(1.0, 0.0, 0.0)}
    };
    std::shared_ptr<LitDrawNode> sphereNode{
        new LitDrawNode { sphereMesh, shader , lighting, glm::vec3(8.0, 9.0, 10.0)}
    };

    std::shared_ptr<LitDrawNode> torusNode{
        new LitDrawNode { torusMesh, shader , lighting, glm::vec3(1.0, 0.0, 0.0)}
    };

    // Orbit animation
    std::shared_ptr<SimpleAnimationNode> orbitAnim{ new SimpleAnimationNode { 0.5f, vec3(0, 1, 0)} };
    rootNode.childNodes.push_back(orbitAnim);



    // robot root
    std::shared_ptr<SceneGraphNode> robotRoot{ new SceneGraphNode };
    orbitAnim->childNodes.push_back(robotRoot);
    robotRoot->localTransform = translate(vec3(0, 0, -5));



    std::shared_ptr<SimpleAnimationNode> sphereAnim{ new SimpleAnimationNode { 2.0f, vec3(.2, 0, 1)} };
    robotRoot->childNodes.push_back(sphereAnim);

    // body sphere node
    std::shared_ptr<SceneGraphNode> bodyNode{ new SceneGraphNode };
    sphereAnim->childNodes.push_back(bodyNode);
    bodyNode->localTransform = scale(vec3(.75, 0.75, 0.75));
    bodyNode->childNodes.push_back(sphereNode);



    std::shared_ptr<CustomAnimationNode> headAnim{ new CustomAnimationNode { 1.0f, vec3(0, 1, 0)} };
    robotRoot->childNodes.push_back(headAnim);
    headAnim->localTransform = translate(vec3(0, 1.0, 0)) * scale(vec3(1.2, .7, .7));
    


	// head node
	std::shared_ptr<SceneGraphNode> headNode{ new SceneGraphNode };
    headAnim->childNodes.push_back(headNode);
	headNode->childNodes.push_back(cubeNode); 
  


    std::shared_ptr<SceneGraphNode> leftEye{ new SceneGraphNode };
	headNode->childNodes.push_back(leftEye);
	leftEye->localTransform = rotate(radians(90.0f),vec3(0,0,1))* rotate(radians(90.0f), vec3(1, 0, 0)) *translate(vec3(0, 1.1, .4)) * scale(vec3(.5, .25, .25));
    leftEye->childNodes.push_back(torusNode);
    
    //torusNode->pLight.color = vec3(1, 1, 0.5);
    

    std::shared_ptr<SceneGraphNode> rightEye{ new SceneGraphNode };
    headNode->childNodes.push_back(rightEye);
    rightEye->localTransform = rotate(radians(90.0f), vec3(0, 0, 1)) * rotate(radians(90.0f), vec3(1, 0, 0)) * translate(vec3(0, 1.1, -.4)) * scale(vec3(.5,.25,.25));
    rightEye->childNodes.push_back(torusNode);
    //// tail
    //std::shared_ptr<SceneGraphNode> tailNode{ new SceneGraphNode };
    //helicopterRoot->childNodes.push_back(tailNode);
    //tailNode->localTransform =
    //    translate(vec3(1.5, 0.0, 0.0)) * scale(vec3(0.5, 0.25, 0.25));
    //tailNode->childNodes.push_back(cubeNode);

    //// rotor
    //std::shared_ptr<SceneGraphNode> rotorRoot{ new SceneGraphNode };
    //rotorRoot->localTransform = translate(vec3(0, 1, 0));
    //helicopterRoot->childNodes.push_back(rotorRoot);

    //// Rotor animation
    //std::shared_ptr<CustomAnimationNode> rotorAnim{ new CustomAnimationNode { 1.0f, vec3(0, 1, 0)} };
    //rotorRoot->childNodes.push_back(rotorAnim);

    //// rotor blade
    //for (int i{ 0 }; i < 4; i++)
    //{
    //    std::shared_ptr<SceneGraphNode> rotorBlade{ new SceneGraphNode };
    //    rotorAnim->childNodes.push_back(rotorBlade);
    //    rotorBlade->localTransform =
    //        rotate(radians(i * 90.0f), vec3(0, 1, 0))
    //        * translate(vec3(1.125, 0, 0))
    //        * scale(vec3(1.0, 0.05, 0.125));
    //    rotorBlade->childNodes.push_back(cubeNode);
    //}

    //// tail rotor
    //std::shared_ptr<SceneGraphNode> tailRotorRoot{ new SceneGraphNode };
    //tailRotorRoot->localTransform =
    //    translate(vec3(1.75, 0.0, 0.3))
    //    * rotate(radians(90.0f), vec3(1, 0, 0))
    //    * scale(vec3(0.25));
    //tailRotorRoot->childNodes.push_back(rotorAnim);
    //helicopterRoot->childNodes.push_back(tailRotorRoot);

    //ground
	std::shared_ptr<SceneGraphNode> groundNode{ new SceneGraphNode };
	rootNode.childNodes.push_back(groundNode);
    groundNode->localTransform = translate(vec3(0, -1, 0)) * scale(vec3(10, .1, 10));
    groundNode->childNodes.push_back(cubeNode);

   spotlightNode = shared_ptr<SpotLightNode>{ new SpotLightNode{} };
	//leftEye->childNodes.push_back(spotlightNode);
 //   spotlightNode->localTransform = rotate(radians(90.0f), vec3(1, 0, 0));

	//spotlightNode->spotLight.color = vec3(1.0f, 1.0f, 0.5f);
	//spotlightNode->spotLight.direction = vec3(0, radians(90.0f), 0);
	//spotlightNode->spotLight.intensity = 1.0f;//increase again when you want to deal with it

}

void MySceneGraph::update(float dt)
{
	rootNode.updateSceneGraph(dt);
    lighting.spotLight = spotlightNode->spotLight;
}
void MySceneGraph::draw(const CameraMatrices& camera)
{
	rootNode.drawSceneGraph(camera);
}

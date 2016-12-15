#include "SceneText.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include <sstream>
#include "KeyboardController.h"
#include "MouseController.h"
#include "SceneManager.h"
#include "GraphicsManager.h"
#include "ShaderProgram.h"
#include "EntityManager.h"

#include "GenericEntity.h"
#include "GroundEntity.h"
#include "TextEntity.h"
#include "SpriteEntity.h"
#include "Light.h"
#include "SkyBox/SkyBoxEntity.h"

#include "RenderHelper.h"
#include "../Source/Sound/SoundManager.h"

#include <iostream>
using namespace std;

SceneText* SceneText::sInstance = new SceneText(SceneManager::GetInstance());

SceneText::SceneText()
{
}

SceneText::SceneText(SceneManager* _sceneMgr)
{
	_sceneMgr->AddScene("Start", this);
}

SceneText::~SceneText()
{
    CSceneGraph::GetInstance()->Destroy();
}

void SceneText::Init()
{
	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	currProg = GraphicsManager::GetInstance()->LoadShader("default", "Shader//Texture.vertexshader", "Shader//Texture.fragmentshader");
	
	// Tell the shader program to store these uniform locations
	currProg->AddUniform("MVP");
	currProg->AddUniform("MV");
	currProg->AddUniform("MV_inverse_transpose");
	currProg->AddUniform("material.kAmbient");
	currProg->AddUniform("material.kDiffuse");
	currProg->AddUniform("material.kSpecular");
	currProg->AddUniform("material.kShininess");
	currProg->AddUniform("lightEnabled");
	currProg->AddUniform("numLights");
	currProg->AddUniform("lights[0].type");
	currProg->AddUniform("lights[0].position_cameraspace");
	currProg->AddUniform("lights[0].color");
	currProg->AddUniform("lights[0].power");
	currProg->AddUniform("lights[0].kC");
	currProg->AddUniform("lights[0].kL");
	currProg->AddUniform("lights[0].kQ");
	currProg->AddUniform("lights[0].spotDirection");
	currProg->AddUniform("lights[0].cosCutoff");
	currProg->AddUniform("lights[0].cosInner");
	currProg->AddUniform("lights[0].exponent");
	currProg->AddUniform("lights[1].type");
	currProg->AddUniform("lights[1].position_cameraspace");
	currProg->AddUniform("lights[1].color");
	currProg->AddUniform("lights[1].power");
	currProg->AddUniform("lights[1].kC");
	currProg->AddUniform("lights[1].kL");
	currProg->AddUniform("lights[1].kQ");
	currProg->AddUniform("lights[1].spotDirection");
	currProg->AddUniform("lights[1].cosCutoff");
	currProg->AddUniform("lights[1].cosInner");
	currProg->AddUniform("lights[1].exponent");
	currProg->AddUniform("colorTextureEnabled");
	currProg->AddUniform("colorTexture");
	currProg->AddUniform("textEnabled");
	currProg->AddUniform("textColor");
	
	// Tell the graphics manager to use the shader we just loaded
	GraphicsManager::GetInstance()->SetActiveShader("default");

	lights[0] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[0]", lights[0]);
	lights[0]->type = Light::LIGHT_DIRECTIONAL;
	lights[0]->position.Set(0, 20, 0);
	lights[0]->color.Set(1, 1, 1);
	lights[0]->power = 1;
	lights[0]->kC = 1.f;
	lights[0]->kL = 0.01f;
	lights[0]->kQ = 0.001f;
	lights[0]->cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0]->cosInner = cos(Math::DegreeToRadian(30));
	lights[0]->exponent = 3.f;
	lights[0]->spotDirection.Set(0.f, 1.f, 0.f);
	lights[0]->name = "lights[0]";

	lights[1] = new Light();
	GraphicsManager::GetInstance()->AddLight("lights[1]", lights[1]);
	lights[1]->type = Light::LIGHT_DIRECTIONAL;
	lights[1]->position.Set(1, 1, 0);
	lights[1]->color.Set(1, 1, 0.5f);
	lights[1]->power = 0.4f;
	lights[1]->name = "lights[1]";

	currProg->UpdateInt("numLights", 1);
	currProg->UpdateInt("textEnabled", 0);
	
	// Create the playerinfo instance, which manages all information about the player
	playerInfo = CPlayerInfo::GetInstance();
	playerInfo->Init();

	// Create and attach the camera to the scene
	//camera.Init(Vector3(0, 0, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
	camera.Init(playerInfo->GetPos(), playerInfo->GetTarget(), playerInfo->GetUp());
	playerInfo->AttachCamera(&camera);
	GraphicsManager::GetInstance()->AttachCamera(&camera);

	// Load all the meshes
	MeshBuilder::GetInstance()->GenerateAxes("reference");
	MeshBuilder::GetInstance()->GenerateQuad("quad", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("quad")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GenerateText("text", 16, 16);
	MeshBuilder::GetInstance()->GetMesh("text")->textureID = LoadTGA("Image//calibri.tga");
	MeshBuilder::GetInstance()->GetMesh("text")->material.kAmbient.Set(1, 0, 0);
	MeshBuilder::GetInstance()->GenerateOBJ("Chair", "OBJ//chair.obj");
	MeshBuilder::GetInstance()->GetMesh("Chair")->textureID = LoadTGA("Image//chair.tga");
	MeshBuilder::GetInstance()->GenerateRing("ring", Color(1, 0, 1), 36, 1, 0.5f);
	MeshBuilder::GetInstance()->GenerateSphere("lightball", Color(1, 1, 1), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateSphere("sphere", Color(1, 0, 0), 18, 36, 1.f);
	MeshBuilder::GetInstance()->GenerateCone("cone", Color(0.5f, 1, 0.3f), 36, 10.f, 10.f);
	MeshBuilder::GetInstance()->GenerateCube("cube", Color(1.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kDiffuse.Set(0.99f, 0.99f, 0.99f);
	MeshBuilder::GetInstance()->GetMesh("cone")->material.kSpecular.Set(0.f, 0.f, 0.f);
	//MeshBuilder::GetInstance()->GenerateQuad("GRASS_DARKGREEN", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("GRASS_DARKGREEN")->textureID = LoadTGA("Image//grass_darkgreen.tga");
	//MeshBuilder::GetInstance()->GenerateQuad("GEO_GRASS_LIGHTGREEN", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("GEO_GRASS_LIGHTGREEN")->textureID = LoadTGA("Image//grass_lightgreen.tga");

	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_FRONT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BACK", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_LEFT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_RIGHT", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_TOP", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("SKYBOX_BOTTOM", Color(1, 1, 1), 1.f);
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//skybox_front.tga");
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//skybox_back.tga");
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//skybox_left.tga");
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//skybox_right.tga");
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//skybox_top.tga");
	//MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//skybox_bottom.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_FRONT")->textureID = LoadTGA("Image//SkyBox//space_front.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BACK")->textureID = LoadTGA("Image//SkyBox//space_back.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_LEFT")->textureID = LoadTGA("Image//SkyBox//space_left.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_RIGHT")->textureID = LoadTGA("Image//SkyBox//space_right.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_TOP")->textureID = LoadTGA("Image//SkyBox//space_top.tga");
	MeshBuilder::GetInstance()->GetMesh("SKYBOX_BOTTOM")->textureID = LoadTGA("Image//SkyBox//space_bottom.tga");
    MeshBuilder::GetInstance()->GenerateRay("laser", 10.0f);
    MeshBuilder::GetInstance()->GenerateCube("cubeSG", Color(1.0f, 0.64f, 0.0f), 1.0f);
    MeshBuilder::GetInstance()->GenerateQuad("GRIDMESH", Color(1.0f, 1.0f, 1.0f), 10.f);
    MeshBuilder::GetInstance()->GenerateCube("CUBEMESH", Color(1.0f, 1.0f, 1.0f), 1.f);
    MeshBuilder::GetInstance()->GenerateCube("CUBEMESH2", Color(1.0f, 0.0f, 0.0f), 1.f);

	MeshBuilder::GetInstance()->GenerateCube("hitbox", Color(1.0f, 0.0f, 0.0f), 1.0f);

    // Asteroid
	MeshBuilder::GetInstance()->GenerateOBJ("Asteroid1", "OBJ//Asteroids//Asteroid1.obj");
	MeshBuilder::GetInstance()->GetMesh("Asteroid1")->textureID = LoadTGA("Image//Asteroids//Asteroid.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Asteroid2", "OBJ//Asteroids//Asteroid2.obj");
	MeshBuilder::GetInstance()->GetMesh("Asteroid2")->textureID = LoadTGA("Image//Asteroids//Asteroid.tga");

    // Defense
    MeshBuilder::GetInstance()->GenerateOBJ("DDrone", "OBJ//DDrone.obj");
    MeshBuilder::GetInstance()->GetMesh("DDrone")->textureID = LoadTGA("Image//DDrone.tga");
	MeshBuilder::GetInstance()->GenerateOBJ("Base1", "OBJ//Base//base.obj");
	MeshBuilder::GetInstance()->GetMesh("Base1")->textureID = LoadTGA("Image//Base//base.tga");

    // Player & Minimap
	MeshBuilder::GetInstance()->GenerateQuad("crosshair", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("crosshair")->textureID = LoadTGA("Image//crosshair.tga");
	//MeshBuilder::GetInstance()->GenerateOBJ("Machinegun", "OBJ//MachineGun.obj");
	//MeshBuilder::GetInstance()->GetMesh("Machinegun")->textureID = LoadTGA("Image//MachineGun.tga");
	MeshBuilder::GetInstance()->GenerateQuad("MachinegunL", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("MachinegunL")->textureID = LoadTGA("Image//MachineGunL.tga");
	MeshBuilder::GetInstance()->GenerateQuad("MachinegunR", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("MachinegunR")->textureID = LoadTGA("Image//MachineGunR.tga");

	MeshBuilder::GetInstance()->GenerateQuad("HP", Color(0, 1, 0), 1.f);
	MeshBuilder::GetInstance()->GenerateQuad("HUD", Color(1, 1, 1), 1.f);
	MeshBuilder::GetInstance()->GetMesh("HUD")->textureID = LoadTGA("Image//HUD.tga");
	MeshBuilder::GetInstance()->GenerateQuad("Minimap", Color(1.0f, 1.0f, 1.0f), 1.0f);
	MeshBuilder::GetInstance()->GetMesh("Minimap")->textureID = LoadTGA("Image//minimap_bg.tga");
	MeshBuilder::GetInstance()->GenerateMinimapBorder("Border", Color(1.0f, 1.0f, 1.0f), 1.0f);
	MeshBuilder::GetInstance()->GenerateCircle("Player", Color(0.0f, 1.0f, 0.0f), 1.0f);
	MeshBuilder::GetInstance()->GenerateCircle("Enemy", Color(1.0f, 0.0f, 0.0f), 1.0f);

    // Enemies
    MeshBuilder::GetInstance()->GenerateOBJ("Fighter", "OBJ//fighter.obj");
    MeshBuilder::GetInstance()->GetMesh("Fighter")->textureID = LoadTGA("Image//fighter.tga");
    MeshBuilder::GetInstance()->GenerateOBJ("DShip", "OBJ//DShip.obj");
    //MeshBuilder::GetInstance()->GetMesh("DShip")->textureID = LoadTGA("Image//DShip.tga");

	SoundManager::GetInstance()->playMusic("Sound/BGM.mp3");
    
    // Set up the Spatial Partition and pass it to the EntityManager to manage
    CSpatialPartition::GetInstance()->Init(800, 800, 800, 4, 4, 4);
    //CSpatialPartition::GetInstance()->SetMesh("CUBEMESH", "CUBEMESH2");
    CSpatialPartition::GetInstance()->SetCamera(&camera);
    CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 160000.0f);
    EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

    //// Set up the Spatial Partition and pass it to the EntityManager to manage
    //CSpatialPartition::GetInstance()->Init(250, 250, 250, 4, 4, 4);
    //CSpatialPartition::GetInstance()->SetMesh("CUBEMESH", "CUBEMESH2");
    //CSpatialPartition::GetInstance()->SetCamera(&camera);
    //CSpatialPartition::GetInstance()->SetLevelOfDetails(40000.0f, 160000.0f);
    //EntityManager::GetInstance()->SetSpatialPartition(CSpatialPartition::GetInstance());

	// Create entities into the scene
	Create::Entity("reference", Vector3(0.0f, 0.0f, 0.0f)); // Reference
	Create::Entity("lightball", Vector3(lights[0]->position.x, lights[0]->position.y, lights[0]->position.z)); // Lightball
	//GenericEntity* aCube = Create::Entity("cube", Vector3(-20.0f, 0.0f, -20.0f));
 //   aCube->SetCollider(true);
 //   aCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
 //   aCube->InitLOD("cube", "sphere", "cubeSG");
	//aCube->ShowAABB = true;

    //// Add the pointer to this new entity to the Scene Graph
    //CSceneNode* theNode = CSceneGraph::GetInstance()->AddNode(aCube);
    //if (theNode == NULL)
    //    cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//
    //GenericEntity* anotherCube = Create::Entity("cube", Vector3(-20.0f, 1.1f, -20.0f));
    //anotherCube->SetCollider(true);
    //anotherCube->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
    //CSceneNode* anotherNode = theNode->AddChild(anotherCube);
    //if (anotherCube == NULL)
    //    cout << "EntityManager::AddEntity: Unable to add to scene graph!" << endl;
	//
    //GenericEntity* baseCube = Create::Asset("cube", Vector3(0.0f, 0.0f, 0.0f));
    //CSceneNode* baseNode = CSceneGraph::GetInstance()->AddNode(baseCube);
	//
    //CUpdateTransformation* baseMtx = new CUpdateTransformation();
    //baseMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
    //baseMtx->SetSteps(-60, 60);
    //baseNode->SetUpdateTransformation(baseMtx);
	//
    //GenericEntity* childCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
    //CSceneNode* childNode = baseNode->AddChild(childCube);
    //childNode->ApplyTranslate(0.0f, 1.0f, 0.0f);
	//
    //GenericEntity* grandchildCube = Create::Asset("cubeSG", Vector3(0.0f, 0.0f, 0.0f));
    //CSceneNode* grandchildNode = childNode->AddChild(grandchildCube);
    //grandchildNode->ApplyTranslate(0.0f, 0.0f, 1.0f);
	//
    //CUpdateTransformation* aRotateMtx = new CUpdateTransformation();
    //aRotateMtx->ApplyUpdate(1.0f, 0.0f, 0.0f, 1.0f);
    //aRotateMtx->SetSteps(-120, 60);
    //grandchildNode->SetUpdateTransformation(aRotateMtx);

    //CUpgrade* upgrade = new CUpgrade();
    //upgrade->Init("GRIDMESH");

	groundEntity = Create::Ground("GRASS_DARKGREEN", "GEO_GRASS_LIGHTGREEN");
    // Create::Text3DObject("text", Vector3(0.0f, 0.0f, 0.0f), "DM2210", Vector3(10.0f, 10.0f, 10.0f), Color(0, 1, 1));
	SpriteEntity* crosshair = Create::Sprite2DObject("crosshair", Vector3(0.0f, 0.0f, 1.0f), Vector3(150.0f, 150.0f, 150.0f));

	SkyBoxEntity* theSkyBox = Create::SkyBox("SKYBOX_FRONT", "SKYBOX_BACK",
											 "SKYBOX_LEFT", "SKYBOX_RIGHT",
											 "SKYBOX_TOP", "SKYBOX_BOTTOM");

	// Customise the ground entity
	//groundEntity->SetPosition(Vector3(0, -10, 0));
	//groundEntity->SetScale(Vector3(100.0f, 100.0f, 100.0f));
	//groundEntity->SetGrids(Vector3(10.0f, 1.0f, 10.0f));
	////playerInfo->SetTerrain(groundEntity);
    //theEnemy->SetTerrain(groundEntity);

    EntitySpawn = new CSpawn();

	// Setup the 2D entities
	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;
	float fontSize = 25.0f;
	float halfFontSize = fontSize / 2.0f;
	for (int i = 0; i < 10; ++i)
	{
		textObj[i] = Create::Text2DObject("text", Vector3(-halfWindowWidth, -halfWindowHeight + fontSize*i + halfFontSize, 0.0f), "", Vector3(fontSize, fontSize, fontSize), Color(0.0f,1.0f,0.0f));
	}
	//textObj[0]->SetText("HELLO WORLD");
	textObj[4]->SetText("HP");
	textObj[4]->SetPosition(Vector3(-15, halfWindowHeight - 15, 0.0f));

	LMG = Create::Sprite2DObject("MachinegunL", Vector3(0, 0, 0), Vector3(halfWindowWidth + 150, halfWindowHeight + 150, 1));
	RMG = Create::Sprite2DObject("MachinegunR", Vector3(0, 0, 0), Vector3(halfWindowWidth + 150, halfWindowHeight + 150, 1));

	//Spawning of asteroid
	//for (int i = 0; i < 50; i++)
	//{
	//	Asteroid* asteroids = new Asteroid();
	//	asteroids->Init();
	//}

	Base* base = new Base();
	base->Init();

	HP_Scale = (float) (playerInfo->GetHP() * 4);
	HP_Bar = Create::Sprite2DObject("HP", Vector3(0, halfWindowHeight - 35, 0), Vector3(HP_Scale, 20, 1));
	SpriteEntity* HUD = Create::Sprite2DObject("HUD", Vector3(0, 0, 0), Vector3(halfWindowWidth * 2, halfWindowHeight * 2, 1));
	
	Minimap* minimap = new Minimap();
	minimap->Init();
}

void SceneText::Update(double dt)
{
	// Update our entities
	EntityManager::GetInstance()->Update(dt);

	//std::cout << EntityManager::GetInstance()->entityList.size() << std::endl;

	// THIS WHOLE CHUNK TILL <THERE> CAN REMOVE INTO ENTITIES LOGIC! Or maybe into a scene function to keep the update clean
	if(KeyboardController::GetInstance()->IsKeyDown('1'))
		glEnable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('2'))
		glDisable(GL_CULL_FACE);
	if(KeyboardController::GetInstance()->IsKeyDown('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(KeyboardController::GetInstance()->IsKeyDown('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(KeyboardController::GetInstance()->IsKeyDown('5'))
	{
		lights[0]->type = Light::LIGHT_POINT;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('6'))
	{
		lights[0]->type = Light::LIGHT_DIRECTIONAL;
	}
	else if(KeyboardController::GetInstance()->IsKeyDown('7'))
	{
		lights[0]->type = Light::LIGHT_SPOT;
	}

	if(KeyboardController::GetInstance()->IsKeyDown('I'))
		lights[0]->position.z -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('K'))
		lights[0]->position.z += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('J'))
		lights[0]->position.x -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('L'))
		lights[0]->position.x += (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('O'))
		lights[0]->position.y -= (float)(10.f * dt);
	if(KeyboardController::GetInstance()->IsKeyDown('P'))
		lights[0]->position.y += (float)(10.f * dt);

	// if the left mouse button was released
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::LMB))
	{
		cout << "Left Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::RMB))
	{
		cout << "Right Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->IsButtonReleased(MouseController::MMB))
	{
		cout << "Middle Mouse Button was released!" << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in X-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_XOFFSET) << endl;
	}
	if (MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) != 0.0)
	{
		cout << "Mouse Wheel has offset in Y-axis of " << MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET) << endl;
	}
	// <THERE>

	// Update the player position and other details based on keyboard and mouse inputs
	playerInfo->Update(dt);

    // Spawner
    if (EntitySpawn->GetIsWaveCleared())
        EntitySpawn->Spawner();

	//camera.Update(dt); // Can put the camera into an entity rather than here (Then we don't have to write this)

	GraphicsManager::GetInstance()->UpdateLights(dt);

	// Update the 2 text object values. NOTE: Can do this in their own class but i'm lazy to do it now :P
	// Eg. FPSRenderEntity or inside RenderUI for LightEntity

	float halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2.0f;
	float halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2.0f;

	std::ostringstream ss;
	ss.precision(3);
	float fps = (float)(1.f / dt);
	ss << "FPS:" << fps;
	textObj[0]->SetText(ss.str());
	textObj[0]->SetPosition(Vector3(-halfWindowWidth, halfWindowHeight -10 , 0.0f));

	//POS
	ss.str("");
	ss.precision(4);
	ss << "X:" << playerInfo->GetPos().x;
	textObj[1]->SetText(ss.str());
	textObj[1]->SetPosition(Vector3(-halfWindowWidth + 10, halfWindowHeight - 40, 0.0f));
	ss.str("");
	ss << "Y:" << playerInfo->GetPos().y;
	textObj[2]->SetText(ss.str());
	textObj[2]->SetPosition(Vector3(-halfWindowWidth + 10, halfWindowHeight - 60, 0.0f));
	ss.str("");
	ss << "Z:" << playerInfo->GetPos().z;
	textObj[3]->SetText(ss.str());
	textObj[3]->SetPosition(Vector3(-halfWindowWidth + 10, halfWindowHeight - 80, 0.0f));

	//UI ELEMENTS
	HP_Scale = (float)(playerInfo->GetHP() * 4);
	HP_Bar->SetScale(Vector3(HP_Scale, 20, 1));

	std::ostringstream UI;
	if (playerInfo->LeftMachineGun->reloading)
		UI << "RELOADING";
	else
		UI << "MG:" << playerInfo->LeftMachineGun->GetMagRound() << "/" << playerInfo->LeftMachineGun->GetMaxMagRound();
	textObj[5]->SetText(UI.str());
	textObj[5]->SetPosition(Vector3(0 - 265, -halfWindowHeight + 85, 0.0f));
	textObj[5]->SetAngle(-20.0f);
	
	UI.str("");
	if (playerInfo->RightMachineGun->reloading)
		UI << "RELOADING";
	else
		UI << "MG:" << playerInfo->RightMachineGun->GetMagRound() << "/" << playerInfo->RightMachineGun->GetMaxMagRound();
	textObj[6]->SetText(UI.str());
	textObj[6]->SetPosition(Vector3(0 + 150, -halfWindowHeight + 45, 0.0f));
	textObj[6]->SetAngle(20.0f);

	if (!playerInfo->LeftMachineGun->reloading)
	{
		LMG->SetPosition(Vector3(-330, -120, -1.0f));
		LMG->SetAngle(-5.0f);
	}
	else
	{
		LMG->SetPosition(Vector3(-330, -180, -1.0f));
	}

	if (!playerInfo->RightMachineGun->reloading)
	{
		RMG->SetPosition(Vector3(360, -120, -1.0f));
		RMG->SetAngle(5.0f);
	}
	else
	{
		RMG->SetPosition(Vector3(360, -180, -1.0f));
	}

	//UI.str("");
	//UI << "MSL:" << playerInfo->secondaryWeapon->GetMagRound() << "/" << playerInfo->secondaryWeapon->GetMaxMagRound();
	//textObj[6]->SetText(UI.str());
	//textObj[6]->SetPosition(Vector3(0 + 150, -halfWindowHeight + 30, 0.0f));
	//textObj[6]->SetAngle(15.0f);
}


void SceneText::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GraphicsManager::GetInstance()->UpdateLightUniforms();

	// Setup 3D pipeline then render 3D
	GraphicsManager::GetInstance()->SetPerspectiveProjection(45.0f, 4.0f / 3.0f, 0.1f, 10000.0f);
	GraphicsManager::GetInstance()->AttachCamera(&camera);
	EntityManager::GetInstance()->Render();

	// Setup 2D pipeline then render 2D
	int halfWindowWidth = Application::GetInstance().GetWindowWidth() / 2;
	int halfWindowHeight = Application::GetInstance().GetWindowHeight() / 2;
	GraphicsManager::GetInstance()->SetOrthographicProjection(-halfWindowWidth, halfWindowWidth, -halfWindowHeight, halfWindowHeight, -10, 10);
	GraphicsManager::GetInstance()->DetachCamera();

	//MS& modelStack = GraphicsManager::GetInstance()->GetModelStack();
	//modelStack.PushMatrix();
	//modelStack.Translate(playerInfo->GetPos().x + 2, playerInfo->GetPos().y, playerInfo->GetPos().z);
	//modelStack.Scale(0.5f, 0.5f, 0.5f);
	//RenderHelper::RenderMesh("")
	//modelStack.PopMatrix();

	EntityManager::GetInstance()->RenderUI();
}

void SceneText::Exit()
{
	// Detach camera from other entities
	GraphicsManager::GetInstance()->DetachCamera();
	playerInfo->DetachCamera();

	if (playerInfo->DropInstance() == false)
	{
#if _DEBUGMODE==1
		cout << "Unable to drop PlayerInfo class" << endl;
#endif
	}

	// Delete the lights
	delete lights[0];
	delete lights[1];
}

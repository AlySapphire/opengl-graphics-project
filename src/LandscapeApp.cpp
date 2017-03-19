

/*
-------------------------------------------------------------------------------
INSTRUCTIONS:
-------------------------------------------------------------------------------
STEP 1: Load a shader program
See the LoadShader method

STEP 2: Generate Geometry
See the CreateGeometry method

STEP 3: Each Frame - Render Geometry (using the shader program)
See the DrawGeometry method

STEP 4: Unload Shader and Geometry
-------------------------------------------------------------------------------
*/

#include "LandscapeApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <CustomClasses/Camera.h>
#include <Texture.h>
#include <CustomClasses/Shader.h>
#include "Light.h"
#include <gl_core_4_4.h>
#include <CustomClasses/Oglc.h>
#include "Mesh.h"
#include "Model.h"
#include "FrameBuffer.h"
#include "AABB.h"
#include "FBXFile.h"
#include "Emitter.h"
#include <imgui.h>

typedef unsigned int uint;

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

void LandscapeApp::GetFrustumPlanes(const glm::mat4 & p_Transform, glm::vec4 * p_Planes) {

	//right side
	p_Planes[0] = vec4(p_Transform[0][3] - p_Transform[0][0],
					   p_Transform[1][3] - p_Transform[1][0], 
					   p_Transform[2][3] - p_Transform[2][0], 
					   p_Transform[3][3] - p_Transform[3][0]);

	//left side
	p_Planes[1] = vec4(p_Transform[0][3] + p_Transform[0][0],
					   p_Transform[1][3] + p_Transform[1][0],
					   p_Transform[2][3] + p_Transform[2][0],
					   p_Transform[3][3] + p_Transform[3][0]);

	//top
	p_Planes[2] = vec4(p_Transform[0][3] - p_Transform[0][1],
					   p_Transform[1][3] - p_Transform[1][1],
					   p_Transform[2][3] - p_Transform[2][1],
					   p_Transform[3][3] - p_Transform[3][1]);

	//bottom
	p_Planes[3] = vec4(p_Transform[0][3] + p_Transform[0][1],
					   p_Transform[1][3] + p_Transform[1][1],
					   p_Transform[2][3] + p_Transform[2][1],
					   p_Transform[3][3] + p_Transform[3][1]);

	//far
	p_Planes[4] = vec4(p_Transform[0][3] - p_Transform[0][2],
					   p_Transform[1][3] - p_Transform[1][2],
					   p_Transform[2][3] - p_Transform[2][2],
					   p_Transform[3][3] - p_Transform[3][2]);

	//near
	p_Planes[5] = vec4(p_Transform[0][3] + p_Transform[0][2],
					   p_Transform[1][3] + p_Transform[1][2],
					   p_Transform[2][3] + p_Transform[2][2],
					   p_Transform[3][3] + p_Transform[3][2]);

	//planes normalization based on legth of normal
	for(uint i = 0; i < 6; i++) {
		float d = glm::length(vec3(p_Planes[i]));
		p_Planes[i] /= d;
	}

}

LandscapeApp::LandscapeApp()
	: m_Camera(nullptr), m_Texture(nullptr), m_HeightMap(nullptr), m_Grass(nullptr), m_Model(nullptr), m_Emitter(nullptr),
	m_Snow(nullptr), m_Rock(nullptr), m_SplatMap(nullptr), m_Light(nullptr), m_MainShader(nullptr), m_FBO(nullptr) {

}

LandscapeApp::~LandscapeApp() {

	m_Camera = nullptr;
	m_Texture = nullptr;
	m_HeightMap = nullptr;
	m_Grass = nullptr;
	m_Snow = nullptr;
	m_Rock = nullptr;
	m_SplatMap = nullptr;
	m_Light = nullptr;
	m_MainShader = nullptr;
	m_Model = nullptr;
	m_FBO = nullptr;
	m_Emitter = nullptr;

}

bool LandscapeApp::startup() {
	
	//Load model in
	m_Model = new Model();
	if(!m_Model->Init("./models/goblin/Goblin_Animated_V1_Texture_Fix.fbx"))	quit();

	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	m_Camera = new Camera(this);

	//Set up the camera starting position
	m_Camera->SetPosition(glm::vec3(5.0f));
	m_Camera->LookAt(glm::vec3(0.0f));

	//load texture and heightmap
	m_Texture = new aie::Texture();
	m_Texture->load("textures/Tile.png");
	m_HeightMap = new aie::Texture();
	m_HeightMap->load("textures/ps-e.lg.bmp");
	//m_HeightMap->load("textures/heightmap.bmp");
	m_Grass = new aie::Texture();
	m_Grass->load("textures/grass.png");
	m_Snow = new aie::Texture();
	m_Snow->load("textures/snow.png");
	m_Rock = new aie::Texture();
	m_Rock->load("textures/sand.png");
	m_SplatMap = new aie::Texture();
	m_SplatMap->load("textures/ps-t.lg.jpg");
	
	//Set up light position and color
	m_Light = new Light();
	m_Light->Init(glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f), 30.0f, 0.25f);

	//Initialize our Mesh
	m_Mesh = new Mesh();
	m_Mesh->CreateMesh(90, 90, 0.2f, 3.0f, m_SplatMap, m_HeightMap);
	
	//Initialise and load shaders
	m_MainShader = new Shader();
	m_MainShader->LoadShader("Shaders/basic.vert", "Shaders/basic.frag");

	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);
	
	//Setup our frame buffer
	m_FBO = new FrameBuffer();
	m_FBO->SetUpFrameBuffer(getWindowWidth(), getWindowHeight());

	//Setup our Particle Emitter
	m_Emitter = new Emitter();
	m_Emitter->Init(10000, glm::vec3(0.0f), 0.1f, 5.0f, 5.0f, 20.0f, 1.0f, 0.1f, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return true;
}

void LandscapeApp::shutdown() {

	if(m_Camera != nullptr)		delete m_Camera;
	if(m_Texture != nullptr)	delete m_Texture;
	if(m_Light != nullptr)		delete m_Light;
	if(m_SplatMap != nullptr)	delete m_SplatMap;
	if(m_Grass != nullptr)		delete m_Grass;
	if(m_Snow != nullptr)		delete m_Snow;
	if(m_Rock != nullptr)		delete m_Rock;

	if(m_Mesh != nullptr) {
		m_Mesh->DestroyMesh();
		delete m_Mesh;
	}

	if(m_FBO != nullptr) {
		m_FBO->CleanUpFrameBuffer();
		delete m_FBO;
	}

	if(m_MainShader != nullptr) {
		m_MainShader->UnLoadShader();
		delete m_MainShader;
	}

	if(m_Model != nullptr) {
		m_Model->ShutDown();
		delete m_Model;
	}

	if(m_Emitter != nullptr) {
		m_Emitter->ShutDown();
		delete m_Emitter;
	}


	Gizmos::destroy();
}

void LandscapeApp::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	m_Camera->Update(deltaTime);
	m_FBO->Update();
	m_Emitter->Update();

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	m_Light->SetLightPosition(glm::vec3(glm::sin(time) * 2.0f, m_Light->GetLightPosition().y, glm::cos(time) * 2.0f));
	m_Light->Update();

	m_Mesh->Update();

	//Update our model
	m_Model->Update(deltaTime);

	vec4 planes[6];

	glm::mat4 projview = m_projectionMatrix * m_Camera->GetView();

	GetFrustumPlanes(projview, planes);

	bool visible = m_Camera->isVisible(glm::vec3(0.0f, 10.0f, 1.0f), planes, 0.25f);
	bool modelVisible = m_Camera->isVisible(m_Model->GetCentre(), planes, m_Model->GetCentre()[1]);
	
	ImGui::Begin("FrustumCull");
	if(visible)ImGui::Text("Shpere is Visible");
	else ImGui::Text("Sphere not visible");
	if(modelVisible)ImGui::Text("Model is Visible");
	else ImGui::Text("Model not visible");
	ImGui::End();
}

void LandscapeApp::DrawGrid()
{
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

}

void LandscapeApp::draw() {

	OpenGLCalls* oglc = OpenGLCalls::Instance();
	
	// wipe the screen to the background colour
	clearScreen();

	Gizmos::clear();
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	m_FBO->WriteToBuffer(getWindowWidth(), getWindowHeight());

	m_Mesh->DrawMesh(m_projectionMatrix, m_Camera->GetView(), m_Camera->GetPosition(), m_Light->GetLightPosition(), m_Light->GetLightColor(), m_Light->GetLightSpecColor(), m_Light->GetLightAmbientStrength(), m_Light->GetLightSpecStrength(), m_Grass, m_Rock, m_Snow);

	Gizmos::addSphere(m_Light->GetLightPosition(), 0.25f, 20, 20, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	Gizmos::addSphere(glm::vec3(0.0f, 10.0f, 1.0f), 1.0f, 5, 5, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	//Draw our Model
	m_Model->DrawModel(m_projectionMatrix * m_Camera->GetView());

	//Draw our emitter
	glm::vec3 emitterPos = m_Light->GetLightPosition();
	m_Emitter->Draw(getTime(), m_Camera->GetView(), m_projectionMatrix * m_Camera->GetView(), m_Camera->GetPosition(), &emitterPos);

	DrawGrid();
	Gizmos::draw(m_projectionMatrix * m_Camera->GetView());

	
	m_FBO->WriteToScreen(getWindowWidth(), getWindowHeight());

}
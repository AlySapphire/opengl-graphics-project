#include "Emitter.h"

#include <CustomClasses/Shader.h>
#include <glm/glm.hpp>
#include <gl_core_4_4.h>
#include <imgui.h>

Emitter::Emitter() 
	: m_Particles(nullptr), m_UpdateShader(nullptr), m_DrawShader(nullptr) {
}

Emitter::~Emitter() {

	m_Particles = nullptr;
	m_UpdateShader = nullptr;
	m_DrawShader = nullptr;

}

void Emitter::Init(unsigned int p_MaxParticles, const glm::vec3 & p_Pos, float p_LifeMin, float p_LifeMax, float p_VelMin, float p_VelMax, 
				   float p_StartSize, float p_EndSize, const glm::vec4 & p_StartCol, const glm::vec4 & p_EndCol) {

	//Store all variables passed in
	//Colors
	m_StartColor = p_StartCol;
	m_TempStartColor = p_StartCol;
	m_EndColor = p_EndCol;
	m_TempEndColor = p_EndCol;
	//Sizes
	m_StartSize = p_StartSize;
	m_TempStartSize = p_StartSize;
	m_EndSize = p_EndSize;
	m_TempEndSize = p_EndSize;
	//Velocity
	m_VelocityMin = p_VelMin;
	m_TempVelocityMin = p_VelMin;
	m_VelocityMax = p_VelMax;
	m_TempVelocityMax = p_VelMax;
	//Lifespan
	m_LifespanMin = p_LifeMin;
	m_TempLifespanMin = p_LifeMin;
	m_LifespanMax = p_LifeMax;
	m_TempLifespanMax = p_LifeMax;
	//Max particles
	m_MaxParticles = p_MaxParticles;
	m_TempMaxParticles = p_MaxParticles;
	//Position
	m_Position = p_Pos;

	//Create our particle array
	m_Particles = new GPUParticle[m_MaxParticles];

	//Set our starting buffer
	m_ActiveBuffer = 0;

	//Create our Shader handles
	m_UpdateShader = new Shader();
	m_DrawShader = new Shader();

	//Create our buffers and shaders
	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void Emitter::ShutDown() {

	//Destoy particle array
	if(m_Particles != nullptr)	delete m_Particles;
	//Shutdown and destroy update shader
	if(m_UpdateShader != nullptr) {
		m_UpdateShader->UnLoadShader();
		delete m_UpdateShader;
	}
	//Shutdown and destroy draw shader
	if(m_DrawShader != nullptr) {
		m_DrawShader->UnLoadShader();
		delete m_DrawShader;
	}

	//Clean up buffers
	glDeleteVertexArrays(2, m_Vao);
	glDeleteBuffers(2, m_Vbo);

}

void Emitter::Update() {

	ImGui::Begin("Emitter Options");
	ImGui::SliderInt("Max particles", &m_TempMaxParticles, 100, 10000);
	ImGui::InputFloat("Velocity min", &m_TempVelocityMin);
	ImGui::InputFloat("Velocity max", &m_TempVelocityMax);
	ImGui::InputFloat("Lifespan min", &m_TempLifespanMin);
	ImGui::InputFloat("Lifespan max", &m_TempLifespanMax);
	ImGui::InputFloat("Start size", &m_TempStartSize);
	ImGui::InputFloat("End size", &m_TempEndSize);
	ImGui::SliderFloat4("Start Color", &m_TempStartColor[0], 0.0f, 1.0f);
	ImGui::SliderFloat4("End Color", &m_TempEndColor[0], 0.0f, 1.0f);
	if(ImGui::Button("Recreate"))	RecreateEmitter();
	ImGui::End();

}

void Emitter::Draw(float p_Time, const glm::mat4 & p_CameraTransform, const glm::mat4 & p_ProjectionView, const glm::vec3 & p_CameraPosition, 
				   glm::vec3 * p_EmitterPosition) {

	//Update the particles using transform feedback
	m_UpdateShader->Enable();

	//Bind time information
	glUniform1f(glGetUniformLocation(m_UpdateShader->GetProgramID(), "time"), p_Time);

	float deltaTime = p_Time - m_LastDrawTime;
	m_LastDrawTime = p_Time;

	glUniform1f(glGetUniformLocation(m_UpdateShader->GetProgramID(), "deltaTime"), deltaTime);

	//Bind emitter's position
	if(p_EmitterPosition != nullptr)	m_Position = *p_EmitterPosition;
	glUniform3fv(glGetUniformLocation(m_UpdateShader->GetProgramID(), "emitterPosition"), 1, &m_Position[0]);

	//Disable rasterization
	glEnable(GL_RASTERIZER_DISCARD);

	//Bind the buffer we will update
	glBindVertexArray(m_Vao[m_ActiveBuffer]);

	//Work out the "other" buffer
	uint otherBuffer = (m_ActiveBuffer + 1) % 2;

	//Bind the buffer we will update into as points and begin transform feedback
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_Vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);
	glDrawArrays(GL_POINTS, 0, m_MaxParticles);

	//Disable transform feedback and enable rasterization again
	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	//Draw the particles using the geometry shader to billboard them
	m_UpdateShader->Disable();
	m_DrawShader->Enable();

	//Send uniform variables to the draw shader
	glUniformMatrix4fv(glGetUniformLocation(m_DrawShader->GetProgramID(), "projectionView"), 1, false, &p_ProjectionView[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(m_DrawShader->GetProgramID(), "cameraTransform"), 1, false, &p_CameraTransform[0][0]);
	glUniform3fv(glGetUniformLocation(m_DrawShader->GetProgramID(), "cameraPosition"), 1, &p_CameraPosition[0]);

	//Draw particles in the "other" buffer
	glBindVertexArray(m_Vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_MaxParticles);

	//Unbind our VAO and our program
	glBindVertexArray(0);
	m_DrawShader->Disable();

	//Swap buffers for next frame
	m_ActiveBuffer = otherBuffer;

}

void Emitter::CreateBuffers() {

	//Create OpenGL buffers
	glGenVertexArrays(2, m_Vao);
	glGenBuffers(2, m_Vbo);

	//Setup the first buffer
	glBindVertexArray(m_Vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(GPUParticle), m_Particles, GL_STREAM_DRAW);

	//Setup our vertex attribute pointers for the first buffer
	SetupVertAttribPointers();

	//Setup the second buffer
	glBindVertexArray(m_Vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_MaxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	//Setup our vertex attribute pointers for the second buffer
	SetupVertAttribPointers();

	//Unbind our VAO and buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Emitter::CreateUpdateShader() {

	//Specify the data we will stream back
	const char* varyings[] = { "vPos", "vVel", "vLifetime", "vLifespan" };

	//Create our update shader program
	m_UpdateShader->LoadShader("Shaders/GPUParticleUpdate.vert", varyings, 4);

	//Bind the shader so we can set some uniforms that don't change per frame
	m_UpdateShader->Enable();

	//Bind lifetime minimum and maximum
	glUniform1f(glGetUniformLocation(m_UpdateShader->GetProgramID(), "lifeMin"), m_LifespanMin);
	glUniform1f(glGetUniformLocation(m_UpdateShader->GetProgramID(), "lifeMax"), m_LifespanMax);

	//Unbind the program
	m_UpdateShader->Disable();

}

void Emitter::CreateDrawShader() {

	//Create our draw shader program
	m_DrawShader->LoadShader("Shaders/GPUParticle.vert", "Shaders/GPUParticle.frag", "Shaders/GPUParticle.geom");

	//Bind the shader so we can set some uniforms that don't change per frame
	m_DrawShader->Enable();

	//Bind size information for interpolation that won't change
	glUniform1f(glGetUniformLocation(m_DrawShader->GetProgramID(), "sizeStart"), m_StartSize);
	glUniform1f(glGetUniformLocation(m_DrawShader->GetProgramID(), "sizeEnd"), m_EndSize);

	//Bind color information for interpolation that won't change
	glUniform4fv(glGetUniformLocation(m_DrawShader->GetProgramID(), "colorStart"), 1, &m_StartColor[0]);
	glUniform4fv(glGetUniformLocation(m_DrawShader->GetProgramID(), "colorEnd"), 1, &m_EndColor[0]);

	//Unbind the shader
	m_DrawShader->Disable();

}

void Emitter::SetupVertAttribPointers() {

	glEnableVertexAttribArray(0);	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)0);
	glEnableVertexAttribArray(1);	//Velocity
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(2);	//Lifetime
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(3);	//Lifespan
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(sizeof(float) * 7));

}

void Emitter::RecreateEmitter() {

	//Destroy everything and recreate the emitter from variables received from GUI input

	//Destroy Particle array
	if(m_Particles != nullptr)	delete m_Particles;

	//Shutdown and destroy update shader
	if(m_UpdateShader != nullptr) {
		m_UpdateShader->UnLoadShader();
		delete m_UpdateShader;
	}
	//Shutdown and destroy draw shader
	if(m_DrawShader != nullptr) {
		m_DrawShader->UnLoadShader();
		delete m_DrawShader;
	}

	//Clean up buffers
	glDeleteVertexArrays(2, m_Vao);
	glDeleteBuffers(2, m_Vbo);

	//Store variables passed in
	//Velocities
	m_VelocityMin = m_TempVelocityMin;
	m_VelocityMax = m_TempVelocityMax;
	//Lifespans
	m_LifespanMin = m_TempLifespanMin;
	m_LifespanMax = m_TempLifespanMax;
	//Sizes
	m_StartSize = m_TempStartSize;
	m_EndSize = m_TempEndSize;
	//Colors
	m_StartColor = m_TempStartColor;
	m_EndColor = m_TempEndColor;
	//Maximum amount of particles
	m_MaxParticles = m_TempMaxParticles;

	//Create our particle array
	m_Particles = new GPUParticle[m_MaxParticles];

	//Create our Shader handles
	m_UpdateShader = new Shader();
	m_DrawShader = new Shader();

	//Create our buffers and shaders
	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();

}

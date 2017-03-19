#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Shader;

struct GPUParticle {

	GPUParticle() : lifetime(0), lifespan(0) { }

	glm::vec3 pos;
	glm::vec3 vel;
	float lifetime;
	float lifespan;

};

class Emitter {
public:
	Emitter();
	virtual ~Emitter();

	void Init(unsigned int p_MaxParticles, const glm::vec3 & p_Pos, float p_LifeMin, float p_LifeMax, float p_VelMin, float p_VelMax,
			  float p_StartSize, float p_EndSize, const glm::vec4 & p_StartCol, const glm::vec4 & p_EndCol);
	void ShutDown();

	void Update();

	void Draw(float p_Time, const glm::mat4 & p_CameraTransform, const glm::mat4 & p_ProjectionView, const glm::vec3 & p_CameraPosition,
			  glm::vec3* p_EmitterPosition = nullptr);

protected:

	//Setup functions
	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();
	void SetupVertAttribPointers();

	void RecreateEmitter();

#pragma region Properties
	//Particle properties
	GPUParticle* m_Particles;
	unsigned int m_MaxParticles;

	//Lifespan
	float m_LifespanMin;
	float m_LifespanMax;

	//Velocity
	float m_VelocityMin;
	float m_VelocityMax;

	//Size
	float m_StartSize;
	float m_EndSize;

	//Color
	glm::vec4 m_StartColor;
	glm::vec4 m_EndColor;

	//Position
	glm::vec3 m_Position;

#pragma endregion

#pragma region Temp properties

	int m_TempMaxParticles;
	
	//Lifespan
	float m_TempLifespanMin;
	float m_TempLifespanMax;

	//Velocity
	float m_TempVelocityMin;
	float m_TempVelocityMax;

	//Size
	float m_TempStartSize;
	float m_TempEndSize;

	//Color
	glm::vec4 m_TempStartColor;
	glm::vec4 m_TempEndColor;


#pragma endregion

	unsigned int m_ActiveBuffer;
	unsigned int m_Vao[2];
	unsigned int m_Vbo[2];

	Shader* m_DrawShader;
	Shader* m_UpdateShader;

	float m_LastDrawTime;

};

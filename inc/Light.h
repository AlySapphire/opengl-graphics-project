#pragma once

#include <glm/vec3.hpp>

class Light {
public:
	Light();
	~Light();

	void Init(glm::vec3 p_Pos = glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3 p_LightCol = glm::vec3(1.0f), glm::vec3 p_LightSpecCol = glm::vec3(0.5f),
				float p_LightSpecStrength = 32.0f, float p_LightAmbientStrength = 0.25f);

	//	GETTERS

	const glm::vec3 & GetLightPosition();
	const glm::vec3 & GetLightColor();
	const glm::vec3 & GetLightSpecColor();
	const float & GetLightSpecStrength();
	const float & GetLightAmbientStrength();

	//	SETTERS

	void SetLightPosition(glm::vec3 p_Pos);
	void SetLightColor(glm::vec3 p_Col);
	void SetLightSpecColor(glm::vec3 p_SpecCol);
	void SetLightSpecStrength(float p_SpecStrength);
	void SetLightAmbientStrength(float p_AmbientStrength);

	void Update();

private:

	void RevertToDefaults();

	glm::vec3 m_LightPosition;
	glm::vec3 m_LightColor;
	glm::vec3 m_LightSpecColor;
	float m_LightSpecStrength;
	float m_LightAmbientStrength;

	glm::vec3 m_OrigLightCol;
	glm::vec3 m_OrigSpecColor;
	float m_OriginalLightSpecStrength;
	float m_OrigLightAmbientStrength;

};

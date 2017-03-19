#include "Light.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Light::Light() {
}

Light::~Light() {
}

void Light::Init(glm::vec3 p_Pos, glm::vec3 p_LightCol, glm::vec3 p_LightSpecCol, float p_LightSpecStrength, float p_LightAmbientStrength) {

	SetLightPosition(p_Pos);
	SetLightColor(p_LightCol);
	SetLightSpecColor(p_LightSpecCol);
	SetLightSpecStrength(p_LightSpecStrength);
	SetLightAmbientStrength(p_LightAmbientStrength);

	m_OrigLightCol = p_LightCol;
	m_OrigSpecColor = p_LightSpecCol;
	m_OriginalLightSpecStrength = p_LightSpecStrength;
	m_OrigLightAmbientStrength = p_LightAmbientStrength;

}

const glm::vec3 & Light::GetLightPosition() {
	return m_LightPosition;
}

const glm::vec3 & Light::GetLightColor() {
	return m_LightColor;
}

const glm::vec3 & Light::GetLightSpecColor() {
	return m_LightSpecColor;
}

const float & Light::GetLightSpecStrength() {
	return m_LightSpecStrength;
}

const float & Light::GetLightAmbientStrength() {
	return m_LightAmbientStrength;
}

void Light::SetLightPosition(glm::vec3 p_Pos) {
	m_LightPosition = p_Pos;
}

void Light::SetLightColor(glm::vec3 p_Col) {
	m_LightColor = p_Col;
}

void Light::SetLightSpecColor(glm::vec3 p_SpecCol) {
	m_LightSpecColor = p_SpecCol;
}

void Light::SetLightSpecStrength(float p_SpecStrength) {
	m_LightSpecStrength = p_SpecStrength;
}

void Light::SetLightAmbientStrength(float p_AmbientStrength) {
	m_LightAmbientStrength = p_AmbientStrength;
}

void Light::Update() {

	ImGui::Begin("Light Options");
	ImGui::SliderFloat3("Light Color", &m_LightColor[0], 0.0f, 1.0f);
	ImGui::SliderFloat3("Specular Color", &m_LightSpecColor[0], 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient Strength", &m_LightAmbientStrength, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular Strength", &m_LightSpecStrength, 10.0f, 120.0f);
	if(ImGui::Button("Reset to default"))
		RevertToDefaults();
	ImGui::End();

}

void Light::RevertToDefaults() {

	m_LightColor = m_OrigLightCol;
	m_LightSpecColor = m_OrigSpecColor;
	m_LightAmbientStrength = m_OrigLightAmbientStrength;
	m_LightSpecStrength = m_OriginalLightSpecStrength;

}

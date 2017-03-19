#pragma once

#include <glm/fwd.hpp>
#include "AABB.h"

class Shader;
class FBXFile;

class Model {
public:
	Model();
	virtual ~Model();

	bool Init(const char* p_ModelPath);
	void ShutDown();

	void Update(float p_DeltaTime);
	void DrawModel(const glm::mat4 & p_Projection);

	glm::vec3 GetCentre();

protected:

	void DrawAABB();
	bool m_VisAABB = false;

	void CreateFBXOpenGLBuffers(FBXFile* p_File);
	void CleanupFBXOpenGLBuffers(FBXFile* p_File);

	float m_ModelScale;
	float m_Timer;

	//Model Shader Program
	Shader* m_ModelShader;

	//Bounding-Box
	AABB BoundingBox;
	glm::vec3 m_MaxOrig;
	glm::vec3 m_MinOrig;

	//Model File
	FBXFile* m_Model;

};

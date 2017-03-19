#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

class Camera;
class Shader;
class Light;
class Mesh;
class Model;
class FrameBuffer;
class AABB;
class Emitter;
namespace aie {
	class Texture;
}

class LandscapeApp : public aie::Application {
protected:

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;

	Shader* m_MainShader;

	//Model
	Model* m_Model;

	unsigned int m_VertCount;
	unsigned int m_IndicesCount;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	//Stats for our light
	Light* m_Light;

	Mesh* m_Mesh;

	Camera* m_Camera;

	aie::Texture* m_Texture;
	aie::Texture* m_HeightMap;

	aie::Texture* m_SplatMap;
	aie::Texture* m_Grass;
	aie::Texture* m_Rock;
	aie::Texture* m_Snow;

	FrameBuffer* m_FBO;

	Emitter* m_Emitter;

	void GetFrustumPlanes(const glm::mat4 & p_Transform, glm::vec4* p_Planes);

public:

	LandscapeApp();
	virtual ~LandscapeApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void DrawGrid();

};
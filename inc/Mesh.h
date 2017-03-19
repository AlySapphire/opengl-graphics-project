#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace aie {

	class Texture;

}
class Shader;

class Mesh {
public:
	Mesh();
	virtual ~Mesh();

	void CreateMesh(int p_MeshWidth, int p_MeshLength, float p_VertSeparation, float p_HeightMapHeight, aie::Texture* p_MeshTexture, aie::Texture* p_HeightMapTexture);
	void DestroyMesh();

	void DrawMesh(const glm::mat4 & p_ProjectionView, const glm::mat4 & p_CameraView, const glm::vec3 & p_LightPosition, const glm::vec3 & p_LightColor, const glm::vec3 & p_LightSpecColor,
				  float p_LightAmbientStrength, float p_LightSpecStrength);
	void DrawMesh(const glm::mat4 & p_ProjectionView, const glm::mat4 & p_CameraView, const glm::vec3 & p_CamPos, const glm::vec3 & p_LightPosition, const glm::vec3 & p_LightColor, const glm::vec3 & p_LightSpecColor,
				  float p_LightAmbientStrength, float p_LightSpecStrength, aie::Texture* p_Grass, aie::Texture* p_Sand, aie::Texture* p_Snow);

	void Update();

protected:

	struct Vertex {
		glm::vec4 pos;
		glm::vec2 uv;
		glm::vec3 vNormal;

		static void SetupVertexAttribPointers();
	};

	void RegenerateMesh();
	void SetUpVerts();
	void SetUpIndicies();
	void SetUpOpenGLBuffers();

	int m_MeshWidth;
	int m_MeshLength;
	float m_VertSeparation;
	float m_MaxHeight;

	//Verts and indicies
	std::vector<Vertex> m_Verts;
	std::vector<unsigned short> m_Indicies;
	unsigned int m_VertCount;
	unsigned int m_IndiciesCount;

	//Shaders
	Shader* m_Program;

	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	//Textures
	aie::Texture* m_Texture;
	aie::Texture* m_HeightMap;

};

#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <vector>

namespace aie {
	class Texture;
}
class Shader;


class Skybox {
public:
	Skybox();
	~Skybox();

	//Startup and shutdown functions
	bool CreateSkybox(unsigned int p_Width, unsigned int p_Height);
	void DestroySkybox();

	void Draw(glm::mat4 p_ProjectionView);

private:

	//Make these bool functions?
	void SetupVerts();
	void SetupIndices();
	void SetupOpenGLBuffers();
	void SetupVertAttribPointers();

	//Shader program
	Shader* m_Shader;

	//Textures
	aie::Texture* m_SkyboxTexture;
	aie::Texture* m_HeightMap;

	//OpenGL buffers
	unsigned int m_Vao;
	unsigned int m_Vbo;
	unsigned int m_Ibo;

	struct VertexData {
		glm::vec4 pos;
		glm::vec2 uv;
	};

	unsigned int m_Width;
	unsigned int m_Height;

	std::vector<VertexData> m_Verts;
	std::vector<unsigned short> m_Indices;

	unsigned int m_VertCount;
	unsigned int m_IndicesCount;

	float m_MaxHeight;
	float m_VertSeparation;

};
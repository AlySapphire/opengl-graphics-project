#include "Skybox.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl_core_4_4.h>
#include <Texture.h>
#include <CustomClasses/Shader.h>

#pragma region Using
using std::vector;
#pragma endregion

Skybox::Skybox() : m_Shader(nullptr), m_HeightMap(nullptr), m_SkyboxTexture(nullptr) {
}

Skybox::~Skybox() {

	m_Shader = nullptr;
	m_HeightMap = nullptr;
	m_SkyboxTexture = nullptr;

}

bool Skybox::CreateSkybox(unsigned int p_Width, unsigned int p_Height) {

	//TODO: Add function to initialize our Skybox

	//Store variables passed in
	m_Width = p_Width;
	m_Height = p_Height;

	//Initialize shader program
	m_Shader = new Shader();
	m_Shader->LoadShader("Shaders/LandscapeAppShaders/Skybox/Skybox.vert", "Shaders/LandscapeAppShaders/Skybox/Skybox.frag");

	//Create textures
	m_SkyboxTexture = new aie::Texture();
	m_HeightMap = new aie::Texture();

	//Reserve space in our vectors
	uint indicesCapacity = m_Width * m_Height * 6 * 6;		//Width and height with 6 indices per loop and 6 faces
	uint vertsCapacity = m_Width * m_Height * 6;
	m_Indices.reserve(indicesCapacity);
	m_Verts.reserve(vertsCapacity);

	//TEMPORARY: give default values to vert separation and max height (pass those as paramaters later?)
	m_MaxHeight = 2.5f;
	m_VertSeparation = 0.2f;

	return true;
}

void Skybox::DestroySkybox() {

	//TODO: Add function that destoys OpenGL handles and cleans our allocated memory

	//Shutdown and destroy shader program
	if(m_Shader != nullptr) {
		m_Shader->UnLoadShader();
		delete m_Shader;
	}

	//Destroy textures
	if(m_SkyboxTexture != nullptr)		delete m_SkyboxTexture;
	if(m_HeightMap != nullptr)			delete m_HeightMap;

}

void Skybox::Draw(glm::mat4 p_ProjectionView) {

	//TODO: Add basic draw loop

}

void Skybox::SetupVerts() {

	//TODO: Add setup for an inverted cube
	//Currently this setup only does a mesh on the floor
	//We need to have a setup that can work with all 6 sides of a cube and also know how to
	//invert a face.
	//The bottom mesh is already inverted due to it being mostly copied from the mesh class
	//UV coordinates will also need to be calculated from the correct perspective

	//Grab our heightmap image data
	const UCHAR* pixels = m_HeightMap->getPixels();

	//Setup our verts
	for(uint z = 0; z < m_Height; z++) {

		for(uint x = 0; x < m_Width; x++) {

			//Sample from a pixel in the heightmap that corresponds to the x and z position
			int sampleX = (int)((float)x / m_Width * m_HeightMap->getWidth());
			int sampleZ = (int)((float)z / m_Height * m_HeightMap->getHeight());

			//Access like a 2D array
			int i = sampleZ * m_HeightMap->getWidth() + sampleX;

			//Position of vertex
			float xPos = (x * m_VertSeparation) - (m_Width * m_VertSeparation * 0.5f);
			float yPos = (pixels[i * 3] / 255.0f) * m_MaxHeight;		//*3 so we sample only one color from the RGB bitmap
			float zPos = (z * m_VertSeparation) - (m_Height * m_VertSeparation * 0.5f);

			//UV's
			float u = (float)x / (m_Width - 1);
			float v = (float)z / (m_Height - 1);

			//Feed values into vertex structure
			VertexData vert{
				{ xPos, yPos, zPos, 1.0f },	//Position
				{ u, v }					//Texture coordinates (uv)
			};

			//Store the vertex structure
			m_Verts.push_back(vert);

		}
	}

}

void Skybox::SetupIndices() {

	//TODO: Add setup for assigning indexes to verts

}

void Skybox::SetupOpenGLBuffers() {

	//TODO: Add setup for creating OpenGL buffers and feeding in data

}

void Skybox::SetupVertAttribPointers() {

	//TODO: Give OpenGL our data we will feed into the vertex shader

}

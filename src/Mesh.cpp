#include "Mesh.h"

#include <CustomClasses/Shader.h>
#include <Texture.h>
#include <gl_core_4_4.h>
#include <assert.h>
#include <imgui.h>
#include <Input.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

Mesh::Mesh() 
	: m_Program(nullptr), m_Texture(nullptr), m_HeightMap(nullptr) {
}

Mesh::~Mesh() {
	m_Program = nullptr;
	m_Texture = nullptr;
	m_HeightMap = nullptr;

}

void Mesh::CreateMesh(int p_MeshWidth, int p_MeshLength, float p_VertSeparation, float p_HeightMapHeight, aie::Texture * p_MeshTexture, aie::Texture * p_HeightMapTexture) {

	//Initialize and load shaders
	m_Program = new Shader();
	m_Program->LoadShader("Shaders/basic.vert", "Shaders/basic.frag");

	//Store all variables passed in
	m_Texture = p_MeshTexture;
	m_HeightMap = p_HeightMapTexture;
	m_MaxHeight = p_HeightMapHeight;
	m_MeshWidth = p_MeshWidth;
	m_MeshLength = p_MeshLength;
	m_VertSeparation = p_VertSeparation;

	//Setup our verticies
	SetUpVerts();
	//Setup our indicies
	SetUpIndicies();
	//Setup our VAO and buffers
	SetUpOpenGLBuffers();

}

void Mesh::DestroyMesh() {

	if(m_Program != nullptr) {
		m_Program->UnLoadShader();
		delete m_Program;
	}

	glDeleteVertexArrays(1, &m_Vao);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ibo);

}

void Mesh::DrawMesh(const glm::mat4 & p_ProjectionView, const glm::mat4 & p_CameraView, const glm::vec3 & p_LightPosition, const glm::vec3 & p_LightColor, 
					const glm::vec3 & p_LightSpecColor, float p_LightAmbientStrength, float p_LightSpecStrength) {
}

void Mesh::DrawMesh(const glm::mat4 & p_ProjectionView, const glm::mat4 & p_CameraView, const glm::vec3 & p_CamPos, const glm::vec3 & p_LightPosition,
					const glm::vec3 & p_LightColor, const glm::vec3 & p_LightSpecColor, float p_LightAmbientStrength, float p_LightSpecStrength, 
					aie::Texture * p_Grass, aie::Texture * p_Sand, aie::Texture * p_Snow) {
	
	m_Program->Enable();

	glm::mat4 projectionView = p_ProjectionView * p_CameraView;
	glUniformMatrix4fv(glGetUniformLocation(m_Program->GetProgramID(), "projectionView"), 1, false, glm::value_ptr(projectionView));

	//Setup Texture in OpenGL
	uint splatHandle = m_Texture->getHandle();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, splatHandle);
	glUniform1i(glGetUniformLocation(m_Program->GetProgramID(), "alpha"), 0);

	uint grassHandle = p_Grass->getHandle();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, grassHandle);
	glUniform1i(glGetUniformLocation(m_Program->GetProgramID(), "grass"), 1);

	uint sandHandle = p_Sand->getHandle();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, sandHandle);
	glUniform1i(glGetUniformLocation(m_Program->GetProgramID(), "rock"), 2);

	uint snowHandle = p_Snow->getHandle();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, snowHandle);
	glUniform1i(glGetUniformLocation(m_Program->GetProgramID(), "snow"), 3);

	//Setup light/color information
	glUniform3fv(glGetUniformLocation(m_Program->GetProgramID(), "lightPosition"), 1, &p_LightPosition[0]);
	glUniform3fv(glGetUniformLocation(m_Program->GetProgramID(), "lightColor"), 1, &p_LightColor[0]);
	glUniform3fv(glGetUniformLocation(m_Program->GetProgramID(), "lightSpecColor"), 1, &p_LightSpecColor[0]);
	glUniform1fv(glGetUniformLocation(m_Program->GetProgramID(), "lightAmbientStrength"), 1, &p_LightAmbientStrength);
	glUniform3fv(glGetUniformLocation(m_Program->GetProgramID(), "camPos"), 1, &p_CamPos[0]);
	glUniform1fv(glGetUniformLocation(m_Program->GetProgramID(), "lightSpecStrength"), 1, &p_LightSpecStrength);

	//Bind VAO
	glBindVertexArray(m_Vao);

	//Draw Elements
	glDrawElements(GL_TRIANGLES, m_IndiciesCount, GL_UNSIGNED_SHORT, 0);

	//Unbind VAO
	glBindVertexArray(0);
	
	//Unbind program
	m_Program->Disable();

	//Clean up textures
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void Mesh::Update() {

	ImGui::Begin("Mesh Options");
	ImGui::InputFloat("HeightMap Height", &m_MaxHeight);
	ImGui::InputFloat("Vert separation", &m_VertSeparation);
	if(ImGui::Button("Regenerate"))
		RegenerateMesh();
	ImGui::End();

}


void Mesh::RegenerateMesh() {

	glDeleteVertexArrays(1, &m_Vao);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteBuffers(1, &m_Ibo);

	m_Verts.clear();
	m_Indicies.clear();
	SetUpVerts();
	SetUpIndicies();
	SetUpOpenGLBuffers();

}

void Mesh::SetUpVerts() {

	const UCHAR* pixels = m_HeightMap->getPixels();

	//Create a grid of verts
	for(int z = 0; z < m_MeshLength; z++) {

		for(int x = 0; x < m_MeshWidth; x++) {

			//Sample from a pixel in the heightmap that corresponds to the x and z position
			int sampleX = (int)((float)x / m_MeshWidth * m_HeightMap->getWidth());
			int sampleZ = (int)((float)z / m_MeshLength * m_HeightMap->getHeight());

			int i = sampleZ * m_HeightMap->getWidth() + sampleX;	//Access array like it's a 2D array

			//Position of vertex
			float xPos = (x * m_VertSeparation) - (m_MeshWidth * m_VertSeparation * 0.5f);
			float yPos = (pixels[i * 3] / 255.0f) * m_MaxHeight;		//*3 so we sample only one color from the RGB bitmap
			float zPos = (z * m_VertSeparation) - (m_MeshLength * m_VertSeparation * 0.5f);

			//UV's
			float u = (float)x / (m_MeshWidth - 1);
			float v = (float)z / (m_MeshLength - 1);

			//Feed values into vertex structure
			Vertex vert{
				{ xPos, yPos, zPos, 1.0f },	//Position
				{ u, v },					//Texture coordinates (uv)
				{ 0.0f, 0.0f, 0.0f }		//Normals (we will set these up later)
			};

			//Push the structure in out verticies storage
			m_Verts.push_back(vert);

		}

	}

	m_VertCount = m_Verts.size();

}

void Mesh::SetUpIndicies() {

	//Calculate indicies for triangles
	for(int z = 0; z < m_MeshLength - 1; z++) {

		for(int x = 0; x < m_MeshWidth - 1; x++) {

			int i = z * m_MeshWidth + x;	//The address of the verticies in the single dimension vector

			//Triangulate!
			m_Indicies.push_back(i + 1);				// b--a
			m_Indicies.push_back(i);					// | / 
			m_Indicies.push_back(i + m_MeshWidth);		// c   

			//a
			m_Verts[i + 1].vNormal += glm::cross((glm::vec3)m_Verts.at(i).pos - (glm::vec3)m_Verts[i + 1].pos, (glm::vec3)m_Verts.at(i + m_MeshWidth).pos - (glm::vec3)m_Verts[i + 1].pos);
			//b
			m_Verts[i].vNormal += glm::cross((glm::vec3)m_Verts[i + m_MeshWidth].pos - (glm::vec3)m_Verts[i].pos, (glm::vec3)m_Verts[i + 1].pos - (glm::vec3)m_Verts[i].pos);
			//c
			m_Verts[i + m_MeshWidth].vNormal += glm::cross((glm::vec3)m_Verts[i + 1].pos - (glm::vec3)m_Verts[i + m_MeshWidth].pos, (glm::vec3)m_Verts[i].pos - (glm::vec3)m_Verts[i + m_MeshWidth].pos);

			m_Indicies.push_back(i + 1);				//    a
			m_Indicies.push_back(i + m_MeshWidth);		//  / |
			m_Indicies.push_back(i + m_MeshWidth + 1);	// c--d

			//a
			m_Verts[i + 1].vNormal += glm::cross((glm::vec3)m_Verts[i + m_MeshWidth].pos - (glm::vec3)m_Verts[i + 1].pos, (glm::vec3)m_Verts[i + m_MeshWidth + 1].pos - (glm::vec3)m_Verts[i + 1].pos);
			//c
			m_Verts[i + m_MeshWidth].vNormal += glm::cross((glm::vec3)m_Verts[i + m_MeshWidth + 1].pos - (glm::vec3)m_Verts[i + m_MeshWidth].pos, (glm::vec3)m_Verts[i + 1].pos - (glm::vec3)m_Verts[i + m_MeshWidth].pos);
			//d
			m_Verts[i + m_MeshWidth + 1].vNormal += glm::cross((glm::vec3)m_Verts[i + 1].pos - (glm::vec3)m_Verts[i + m_MeshWidth + 1].pos, (glm::vec3)m_Verts[i + m_MeshWidth].pos - (glm::vec3)m_Verts[i + m_MeshWidth + 1].pos);

		}

	}

	m_IndiciesCount = m_Indicies.size();

	//Normalisze our normals (totally not meta)
	for(auto iter = m_Verts.begin(); iter != m_Verts.end(); iter++)
		glm::normalize(iter->vNormal);


}

void Mesh::SetUpOpenGLBuffers() {

	// Generate the VAO and Bind bind it.
	// Our VBO (vertex buffer object) and IBO (Index Buffer Object) will be "grouped" with this VAO
	// other settings will also be grouped with the VAO. this is used so we can reduce draw calls in the render method.
	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	// Create our VBO and IBO.
	// Then tell Opengl what type of buffer they are used for
	// VBO a buffer in graphics memory to contains our vertices
	// IBO a buffer in graphics memory to contain our indices.
	// Then Fill the buffers with our generated data.
	// This is taking our verts and indices from ram, and sending them to the graphics card
	glGenBuffers(1, &m_Vbo);
	glGenBuffers(1, &m_Ibo);

	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ibo);

	glBufferData(GL_ARRAY_BUFFER, m_VertCount * sizeof(Vertex), &m_Verts[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndiciesCount * sizeof(USHORT), &m_Indicies[0], GL_STATIC_DRAW);

	Vertex::SetupVertexAttribPointers();

	//Unbind things after we've finished using them
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Mesh::Vertex::SetupVertexAttribPointers() { 

	// enable vertex position element
	// notice when we loaded the shader, we described the "position" element to be location 0.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0 (position)
		4,                  // size - how many floats make up the position (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z, w are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)0            // offset - bytes from the beginning of the vertex
	);

	// enable vertex color element
	// notice when we loaded the shader, we described the "color" element to be location 1.
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 1 (UV texture coordinates)
		2,                  // size - how many floats make up the uv (u, v)
		GL_FLOAT,           // type - our u, v are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)(sizeof(float) * 4)            // offset - bytes from the beginning of the vertex
	);

	//Enable vertex normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 2 (normal)
		4,                  // size - how many floats make up the normal (x, y, z, w)
		GL_FLOAT,           // type - our x,y,z are float values
		GL_FALSE,           // normalized? - not used
		sizeof(Vertex),     // stride - size of an entire vertex
		(void*)(sizeof(float) * 6)            // offset - bytes from the beginning of the vertex
	);

}

#include "Model.h"
#include "GLMesh.h"
#include "FBXFile.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <CustomClasses/Shader.h>
#include <gl_core_4_4.h>
#include <imgui.h>
#include <Gizmos.h>

Model::Model()
	: m_ModelShader(nullptr), m_Model(false) {
}

Model::~Model() {

	m_ModelShader = nullptr;
	m_Model = nullptr;

}

bool Model::Init(const char * p_ModelPath) {
	
	//Load in our model shader
	m_ModelShader = new Shader();
	m_ModelShader->LoadShader("Shaders/FBXLoadingBasic.vert", "Shaders/FBXLoadingBasic.frag");

	//Load in our FBX model
	m_Model = new FBXFile();
	if(!m_Model->load(p_ModelPath, FBXFile::UNITS_CENTIMETER)) {
		system("pause");
		return false;
	}

	//Setup our OpenGL buffers
	CreateFBXOpenGLBuffers(m_Model);

	//Set up default model scale
	m_ModelScale = 1.0f;
	//Initialize program run time
	m_Timer = 0.0f;
	//Setup the AABB
	BoundingBox.min = glm::vec3(4.85f, 0.0f, 2.0f) * m_ModelScale;
	BoundingBox.max = glm::vec3(-5.2f, 14.0f, -9.3f) * m_ModelScale;

	m_MaxOrig = BoundingBox.max;
	m_MinOrig = BoundingBox.min;

	return true;
}

void Model::ShutDown() {

	//Cleanup our shader
	if(m_ModelShader != nullptr) {
		m_ModelShader->UnLoadShader();
		delete m_ModelShader;
	}

	//Cleanup our model
	if(m_Model != nullptr) {
		CleanupFBXOpenGLBuffers(m_Model);
		delete m_Model;
	}

}

void Model::Update(float p_DeltaTime) {

	ImGui::Begin("Model Options");
	if(ImGui::SliderFloat("Model Scale", &m_ModelScale, 0.01f, 1.5f)) {
		BoundingBox.min = m_MinOrig * m_ModelScale;
		BoundingBox.max = m_MaxOrig * m_ModelScale;
	}
	ImGui::Checkbox("View AABB", &m_VisAABB);
	//ImGui::InputFloat3("Bounding Box Min", &BoundingBox.min[0]);
	//ImGui::InputFloat3("Bounding Box Max", &BoundingBox.max[0]);
	ImGui::End();

	//Update animation time
	m_Timer += p_DeltaTime;

	//Grab the skeleton and animation we want to use
	FBXSkeleton* skeleton = m_Model->getSkeletonByIndex(0);
	FBXAnimation* animation = m_Model->getAnimationByIndex(0);

	//Evaluate the animation to update bones
	skeleton->evaluate(animation, m_Timer);

	for(uint bone_index = 0; bone_index < skeleton->m_boneCount; bone_index++) {

		skeleton->m_nodes[bone_index]->updateGlobalTransform();

	}

}

void Model::DrawModel(const glm::mat4 & p_Projection) {

	//Setup our scale matrix
	glm::mat4 model(m_ModelScale, 0.0f, 0.0f, 0.0f,
					0.0f, m_ModelScale, 0.0f, 0.0f,
					0.0f, 0.0f, m_ModelScale, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
	
	//Calculate our model view projection
	glm::mat4 modelViewProjection = p_Projection * model;
	
	//Enable our shader program
	m_ModelShader->Enable();

	//Draw our AABB
	if(m_VisAABB)	DrawAABB();

	//Send Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(m_ModelShader->GetProgramID(), "projectionViewWorldMatrix"), 1, GL_FALSE, glm::value_ptr(modelViewProjection));

	//Grab the Skeleton and animation we want to use
	FBXSkeleton* skeleton = m_Model->getSkeletonByIndex(0);
	skeleton->updateBones();
	
	//Send the bone matrix to the shader
	glUniformMatrix4fv(glGetUniformLocation(m_ModelShader->GetProgramID(), "bones"), skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	//Loop through each mesh within the fbx file
	for(uint i = 0; i < m_Model->getMeshCount(); i++) {

		FBXMeshNode* mesh = m_Model->getMeshByIndex(i);
		GLMesh* glData = (GLMesh*)mesh->m_userData;

		//Get the texture from the model
		uint diffuseTexture = m_Model->getTextureByIndex(mesh->m_material->DiffuseTexture);

		//bind the texture and send it to our shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glUniform1i(glGetUniformLocation(m_ModelShader->GetProgramID(), "diffuseTexture"), 0);

		//Draw the mesh
		glBindVertexArray(glData->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

		//Unbind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	//Disable our shader program
	m_ModelShader->Disable();

}

glm::vec3 Model::GetCentre() {
	return (BoundingBox.max + BoundingBox.min) / 2;
}

void Model::DrawAABB() {

	glm::vec3 points[] = {
		{ BoundingBox.min },												//0 Front Bottom Right
		{ BoundingBox.max.x, BoundingBox.min.y, BoundingBox.min.z },		//1 Front Bottom Left
		{ BoundingBox.max.x, BoundingBox.max.y, BoundingBox.min.z },		//2 Front Top Left
		{ BoundingBox.min.x, BoundingBox.max.y, BoundingBox.min.z },		//3 Front Top Right
		{ BoundingBox.max.x, BoundingBox.min.y, BoundingBox.max.z },		//4 Back Bottom Left
		{ BoundingBox.min.x, BoundingBox.min.y, BoundingBox.max.z },		//5 Back Bottom Right
		{ BoundingBox.max },												//6 Back Top Left
		{ BoundingBox.min.x, BoundingBox.max.y, BoundingBox.max.z }		//7 Back Top Right
	};
	//aie::Gizmos::addAABB(BoundingBox.min - BoundingBox.max, glm::vec3(3.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	aie::Gizmos::addLine(points[0], points[1], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Front Bottom
	aie::Gizmos::addLine(points[1], points[2], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Front Left
	aie::Gizmos::addLine(points[0], points[3], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Front Right
	aie::Gizmos::addLine(points[2], points[3], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Front Top

	aie::Gizmos::addLine(points[4], points[5], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Back Bottom
	aie::Gizmos::addLine(points[4], points[6], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Back Left
	aie::Gizmos::addLine(points[5], points[7], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Back Right
	aie::Gizmos::addLine(points[6], points[7], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Back Top

	aie::Gizmos::addLine(points[2], points[6], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Left Top
	aie::Gizmos::addLine(points[1], points[4], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Left Bottom
	aie::Gizmos::addLine(points[3], points[7], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Right Top
	aie::Gizmos::addLine(points[0], points[5], glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));		//Right Bottom

}

void Model::CreateFBXOpenGLBuffers(FBXFile * p_File) {

	// FBX Files contain multiple meshes, each with seperate material information
	// loop through each mesh within the FBX file and cretae VAO, VBO and IBO buffers for each mesh.
	// We can store that information within the mesh object via its "user data" void pointer variable.
	for(uint i = 0; i < p_File->getMeshCount(); i++) {

		//Get the current mesh from file
		FBXMeshNode* fbxMesh = p_File->getMeshByIndex(i);
		GLMesh* glData = new GLMesh();

		//oglc->GenVertArrays(&glData->vao);
		//oglc->BindVertexArray(&glData->vao);
		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);

		//oglc->GenBuffers(&glData->vbo);
		//oglc->GenBuffers(&glData->ibo);
		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);

		//Fill the vbo with our vertices. The FBXLoader conviniently already defined a Vertex structure for us
		//oglc->BindAndFeedBuffer(eARRAYBUFFER, &glData->vbo, fbxMesh->m_vertices.size() * sizeof(FBXVertex), fbxMesh->m_vertices.data(), eSTATICDRAW);
		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBufferData(GL_ARRAY_BUFFER, fbxMesh->m_vertices.size() * sizeof(FBXVertex), fbxMesh->m_vertices.data(), GL_STATIC_DRAW);

		//Fill the ibo with the indicies. fbx meshes can be large, so indicies are stored as an unsigned int
		//oglc->BindAndFeedBuffer(eELEMENTARRAYBUFFER, &glData->ibo, fbxMesh->m_indices.size() * sizeof(uint), fbxMesh->m_indices.data(), eSTATICDRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, fbxMesh->m_indices.size() * sizeof(uint), fbxMesh->m_indices.data(), GL_STATIC_DRAW);

		//oglc->SetUpVertexAttribPointers(3, offsets, sizeof(FBXVertex), norms);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
		glEnableVertexAttribArray(2); // uv
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
		glEnableVertexAttribArray(3); //tangents
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glEnableVertexAttribArray(4); //weights
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		//Attatch our GLMesh object to the m_UserData pointer
		fbxMesh->m_userData = glData;

	}
	
	//Unbind Buffers and vertex array
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

}

void Model::CleanupFBXOpenGLBuffers(FBXFile * p_File) {

	for(uint i = 0; i < p_File->getMaterialCount(); i++) {

		FBXMeshNode* fbxMesh = p_File->getMeshByIndex(i);
		GLMesh* glData = (GLMesh*)fbxMesh->m_userData;

		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);

		delete glData;

	}

}

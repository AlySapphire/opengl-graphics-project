#include "FrameBuffer.h"

#include <gl_core_4_4.h>
#include <CustomClasses/Shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <imgui.h>

FrameBuffer::FrameBuffer()
	:m_BufferShader(nullptr), m_EffectToggle(0) {
}

FrameBuffer::~FrameBuffer() {

	m_BufferShader = nullptr;

}

void FrameBuffer::SetUpFrameBuffer(unsigned int p_WindowWidth, unsigned int p_WindowHeight) {

	//Initialize and load our shader
	m_BufferShader = new Shader();
	m_BufferShader->LoadShader("Shaders/PostTest.vert", "Shaders/PostTest.frag");

	//Setup framebuffer
	glGenFramebuffers(1, &m_Fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);

	glGenTextures(1, &m_FboTexture);
	glBindTexture(GL_TEXTURE_2D, m_FboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, p_WindowWidth, p_WindowHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FboTexture, 0);

	glGenRenderbuffers(1, &m_FboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_FboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, p_WindowWidth, p_WindowHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FboDepth);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	//Unbind buffers
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	SetUpQuad(p_WindowWidth, p_WindowHeight);

}

void FrameBuffer::CleanUpFrameBuffer() {

	//Cleanup Shader
	if(m_BufferShader != nullptr) {
		m_BufferShader->UnLoadShader();
		delete m_BufferShader;
	}

	//Clean up handles
	glDeleteVertexArrays(1, &m_Vao);
	glDeleteBuffers(1, &m_Vbo);
	glDeleteFramebuffers(1, &m_Fbo);
	glDeleteTextures(1, &m_FboTexture);
	glDeleteRenderbuffers(1, &m_FboDepth);

}

void FrameBuffer::Update() {

	ImGui::Begin("Post-Processing");
		ImGui::BeginGroup();
		ImGui::SliderInt("Effect Toggle", &m_EffectToggle, 0, 4);
		if(ImGui::IsItemHovered())
			ImGui::SetTooltip("0 for nothing\n1 for box blur\n2 for distortion\n3 for greyscale\n4 for sepia");
		ImGui::EndGroup();
	ImGui::End();

}

void FrameBuffer::WriteToBuffer(unsigned int p_WindowWidth, unsigned int p_WindowHeight) {

	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
	glViewport(0, 0, p_WindowWidth, p_WindowHeight);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void FrameBuffer::WriteToScreen(unsigned int p_WindowWidth, unsigned int p_WindowHeight) {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, p_WindowWidth, p_WindowHeight);

	glClear(GL_DEPTH_BUFFER_BIT);

	m_BufferShader->Enable();

	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, m_FboTexture);
	
	glUniform1i(glGetUniformLocation(m_BufferShader->GetProgramID(), "target"), 25);
	glUniform1i(glGetUniformLocation(m_BufferShader->GetProgramID(), "toggle"), m_EffectToggle);

	glBindVertexArray(m_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	m_BufferShader->Disable();
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE25);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void FrameBuffer::SetUpQuad(unsigned int p_WindowWidth, unsigned int p_WindowHeight) {

	glm::vec2 halfTexel = 1.0f / glm::vec2(p_WindowWidth, p_WindowHeight) * 0.5f;

	float vertexData[] = {
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
		-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,

		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
		1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
		1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
	};

	glGenVertexArrays(1, &m_Vao);
	glBindVertexArray(m_Vao);

	glGenBuffers(1, &m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

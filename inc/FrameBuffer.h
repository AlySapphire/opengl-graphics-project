#pragma once

class Shader;

class FrameBuffer {
public:
	FrameBuffer();
	virtual ~FrameBuffer();

	//Startup/Shutdown functions
	void SetUpFrameBuffer(unsigned int p_WindowWidth, unsigned int p_WindowHeight);
	void CleanUpFrameBuffer();
	
	void Update();

	//Drawing functions
	void WriteToBuffer(unsigned int p_WindowWidth, unsigned int p_WindowHeight);
	void WriteToScreen(unsigned int p_WindowWidth, unsigned int p_WindowHeight);

protected:

	void SetUpQuad(unsigned int p_WindowWidth, unsigned int p_WindowHeight);

	unsigned int m_Fbo;
	unsigned int m_FboTexture;
	unsigned int m_FboDepth;

	//Frame buffer shader
	Shader* m_BufferShader;
	int m_EffectToggle;

	unsigned int m_Vao;
	unsigned int m_Vbo;

};

//Vertex shader
#version 430

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec2 vTexCoord;

//Outputs to the frag shader
out vec2 fTexCoord;

void main()	{
	
	gl_Position = vPos;
	fTexCoord = vTexCoord;

}
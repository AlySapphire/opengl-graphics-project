//Vertex shader
#version 430

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vVel;
layout(location = 2) in float vLifetime;
layout(location = 3) in float vLifespan;

//Outputs to geometry shader
out vec3 gPos;
out float gLifetime;
out float gLifespan;

void main()	{
	
	gPos = vPos;
	gLifetime = vLifetime;
	gLifespan = vLifespan;

}
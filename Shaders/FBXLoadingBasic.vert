//Vertex Shader
#version 430

layout(location = 0)in vec4 vPos;
layout(location = 1)in vec4 vNormal;
layout(location = 2)in vec2 vUv;
layout(location = 3)in vec4 vTangent;
layout(location = 4)in vec4 vWeights;
layout(location = 5)in vec4 vIndicies;

//Outputs to fragment shader
out vec4 fNormal;
out vec2 fUv;

//Maximum limit on bone array
const int MAX_BONES = 128;

//Uniform variables
uniform mat4 projectionViewWorldMatrix;
uniform mat4 bones[MAX_BONES];

void main()	{
	
	fNormal = vNormal;
	fUv = vUv;

	//Cast the indicies to integer's so they can index an array
	ivec4 index = ivec4(vIndicies);

	vec4 P = bones[index.x] * vPos * vWeights.x;
	P += bones[index.y] * vPos * vWeights.y;
	P += bones[index.z] * vPos * vWeights.z;
	P += bones[index.w] * vPos * vWeights.w;

	gl_Position = projectionViewWorldMatrix * P;

}
//Vertex shader for skybox
#version 430	//Using OpenGL 4.3

//Inputs from program
layout(location = 0)in vec4 vPos;
layout(location = 1)in vec2 vUv;

//Outputs to fragment shader
out vec3 fPos;
out vec2 fUv;

//Uniform variables
uniform mat4 projectionView;

void main() {
	
	fPos = vPos.xyz;
	fUv = vUv;
	gl_Position = projectionView * vPos;

}
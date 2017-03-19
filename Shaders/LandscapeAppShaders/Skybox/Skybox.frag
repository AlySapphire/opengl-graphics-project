//Fragment shader for skybox
#version 430	//Using OpenGL 4.3

//Inputs from vertex shader
in vec3 fPos;
in vec2 fUv;

//Outputs to pixel
out vec4 fragColor;

//Uniform variables
uniform sampler2D skyboxTexture;

void main() {
	
	fragColor = texture(skyboxTexture, fUv);

}
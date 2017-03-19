//Fragment shader
#version 430

//Inputs from vertex shader
in vec4 fNormal;
in vec2 fUv;

//Output to pixel
out vec4 FragColor;

//Uniform variables
uniform sampler2D diffuseTexture;

void main()	{
	
	//FragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
	FragColor = texture(diffuseTexture, fUv) * vec4(1.0f, 1.0f, 1.0f, 1.0f);

}
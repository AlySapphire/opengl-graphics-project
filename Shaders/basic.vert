#version 430								
	layout (location = 0)in vec4 vPosition;							
	layout (location = 1)in vec2 vUv;
	layout (location = 2)in vec3 vNormal;
	out vec3 fPos;
	out vec3 fNormal;									
	out vec2 fUv;									
	uniform mat4 projectionView;					
	void main ()									
	{
	 	fPos = vPosition.xyz;
		fNormal = vNormal;										
		fUv = vUv;									
		gl_Position = projectionView * vPosition;	
	}
//Vertex shader
#version 430

layout(location = 0) in vec3 vuPos;
layout(location = 1) in vec3 vuVel;
layout(location = 2) in float vuLifetime;
layout(location = 3) in float vuLifespan;

//Output to our draw vertex shader
out vec3 vPos;
out vec3 vVel;
out float vLifetime;
out float vLifespan;

//Uniform variables
uniform float time;
uniform float deltaTime;
uniform float lifeMin;
uniform float lifeMax;
uniform vec3 emitterPosition;

const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;

//Random function
float rand(uint seed, float range)	{
	
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;

}

void main()	{
	
	vPos = vuPos + vuVel * deltaTime;
	vVel = vuVel;
	vLifetime = vuLifetime + deltaTime;
	vLifespan = vuLifespan;

	//Emit a new particle as soon as it dies
	if(vLifetime > vLifespan)	{
		
		uint seed = uint(time * 1000.0f) + uint(gl_VertexID);
		vVel.x = rand(seed++, 2) - 1;
		vVel.y = rand(seed++, 2) - 1;
		vVel.z = rand(seed++, 2) - 1;
		vVel = normalize(vVel);
		vPos = emitterPosition;
		vLifetime = 0.0f;
		vLifespan = rand(seed++, lifeMax - lifeMin) + lifeMin;

	}

}
#version 430								
	in vec2 fUv;
	in vec3 fPos;
	in vec3 fNormal;				
	out vec4 frag_color;						
	uniform sampler2D texturex;
	uniform sampler2D alpha;
	uniform sampler2D grass;
	uniform sampler2D rock;
	uniform sampler2D snow;

	uniform float lightAmbientStrength;
	uniform vec3 lightPosition;
	uniform vec3 lightColor;	
	uniform vec3 lightSpecColor;
	uniform float lightSpecStrength;
	uniform vec3 camPos;		
	void main ()								
	{
		//Ambient lighting
		vec3 ambient = lightColor * lightAmbientStrength;	//Calculate our ambient lighting by multiplying our light color by the ambient strength

		//Diffuse lighting
		vec3 norm = normalize(fNormal);						//Normalize our normal (totally not meta)
		vec3 lightDir = normalize(lightPosition - fPos);	//Get a direction vector for our light direction
		float diff = max(dot(norm, lightDir), 0.0f);		//Calculate our diffuse with our normal and our direction vector
		vec3 diffColor = diff * lightColor;					//Calculate our final diffuse color by multiplying our diffuse with our light color
		
		//Specular lighting
		vec3 surfToEye = normalize(fPos - camPos);						//Get a direction vector between the position and the camera
		vec3 reflection = reflect(lightDir, norm);						//Get a reflection vector of our light from the light direction and normal
		vec3 spec = pow(max(dot(reflection, surfToEye), 0.0f), lightSpecStrength) * lightSpecColor;	//Final spec value resulting from algorithm

		//Texture splat-mapping
		vec4 sp = texture(alpha, fUv);
		vec4 texColor = sp.r * texture(snow, fUv * 10.0f);
		texColor += sp.g * texture(grass, fUv * 5.0f);
		texColor += sp.b * texture(rock, fUv * 5.0f);
		texColor.w = 1.0f;

		//Final result
		frag_color = texColor * vec4(ambient + diffColor + spec, 1.0f);

		//vec4 Alpha = texture(alpha, fUv);
		//vec4 tex0 = texture(grass, fUv * 4.0f);
		//vec4 tex1 = texture(rock, fUv * 4.0f);
		//vec4 tex2 = texture(snow, fUv * 4.0f);

		//tex0 *= Alpha.r;
		//tex1 = mix(tex0, tex1, Alpha.g);
		//vec4 snowMix = mix(tex1, tex2, Alpha.b)

		//frag_color = snowMix * vec4(ambient + diffColor + spec, 1.0f);

		//Final result
		//frag_color = texture(texturex, fUv) * vec4(ambient + diffColor + spec, 1.0);
	}
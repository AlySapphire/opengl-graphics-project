//Fragment shader
#version 430

//Inputs from vertex shader
in vec2 fTexCoord;

//Outputs to program
out vec4 fragColor;

//Uniform variables
uniform sampler2D target;
uniform int toggle;

//Just sample the target and return the color
vec4 Simple() {
	 return texture(target, fTexCoord);
}

//Simple box blur
vec4 BoxBlur()	{
	
	vec2 texel = 1.0f / textureSize(target, 0).xy;

	//9-tap box kernel
	vec4 color = texture(target, fTexCoord);
	color += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	color += texture(target, fTexCoord + vec2(-texel.x, 0)); 
	color += texture(target, fTexCoord + vec2(-texel.x, -texel.y)); 
	color += texture(target, fTexCoord + vec2(0, texel.y)); 
	color += texture(target, fTexCoord + vec2(0, -texel.y)); 
	color += texture(target, fTexCoord + vec2(texel.x, texel.y)); 
	color += texture(target, fTexCoord + vec2(texel.x, 0)); 
	color += texture(target, fTexCoord + vec2(texel.x, -texel.y));

	return color / 9;

}

//Distortion effect
vec4 Distort()	{
	
	vec2 mid = vec2(0.5f);

	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre + sin(distanceFromCentre * 15) * 0.02f;

	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(target, newCoord);

}

//GrayScale effect
vec4 GrayScale() {

	vec4 color = texture(target, fTexCoord);
	float gray = (color.r + color.g + color.b) / 3.0f;
	vec3 grayscale = vec3(gray);
	
	return vec4(grayscale, color.a);

}

//Sepia effect
vec4 Sepia() {

	//Grab the normal output
	vec4 color = Simple();

	//Clamp the boundries
	float x = clamp(color.r * 0.393f + color.g * 0.769f + color.b * 0.189f, 0.0f, 1.0f);
	float y = clamp(color.r * 0.349f + color.g * 0.686f + color.b * 0.168f, 0.0f, 1.0f);
	float z = clamp(color.r * 0.272f + color.g * 0.534f + color.b * 0.131f, 0.0f, 1.0f);

	//Return the sepia output
	return vec4(x, y, z, color.a);

}

void main()	{
	
	switch(toggle) {
		case 0:
			fragColor = Simple();
			break;
		case 1:
			fragColor = BoxBlur();
			break;
		case 2:
			fragColor = Distort();
			break;
		case 3:
			fragColor = GrayScale();
			break;
		case 4:
			fragColor = Sepia();
			break;
	}

}
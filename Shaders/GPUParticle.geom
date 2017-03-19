//Geometry Shader
#version 430

//Input is points but output a quad
layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

//Input data from the vertex shader
in vec3 gPos[];
in float gLifetime[];
in float gLifespan[];

//Output to fragment shader
out vec4 fColor;

//Uniform variables
uniform mat4 projectionView;
uniform mat4 cameraTransform;
uniform vec3 cameraPosition;

uniform float sizeStart;
uniform float sizeEnd;

uniform vec4 colorStart;
uniform vec4 colorEnd;

void main()	{
	
	//Interpolate color
	fColor = mix(colorStart, colorEnd, gLifetime[0] / gLifespan[0]);

	//Calculate the size and create the corners of a quad
	float halfSize = mix(sizeStart, sizeEnd, gLifetime[0] / gLifespan[0]) * 0.5f;

	vec3 corners[4];
	corners[0] = vec3(halfSize, -halfSize, 0);
	corners[1] = vec3(halfSize, halfSize, 0);
	corners[2] = vec3(-halfSize, -halfSize, 0);
	corners[3] = vec3(-halfSize, halfSize, 0);

	//Billboard
	vec3 zAxis = normalize(cameraPosition - gPos[0]);
	vec3 xAxis = cross(cameraTransform[1].xyz, zAxis);
	vec3 yAxis = cross(zAxis, xAxis);
	mat3 billboard = mat3(xAxis, yAxis, zAxis);

	//Emit the 4 verticies for the quad
	gl_Position = projectionView * vec4(billboard * corners[0] + gPos[0], 1);
	EmitVertex();
	gl_Position = projectionView * vec4(billboard * corners[1] + gPos[0], 1);
	EmitVertex();
	gl_Position = projectionView * vec4(billboard * corners[2] + gPos[0], 1);
	EmitVertex();
	gl_Position = projectionView * vec4(billboard * corners[3] + gPos[0], 1);
	EmitVertex();

}
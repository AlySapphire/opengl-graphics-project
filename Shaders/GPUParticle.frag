//Fragment shader
#version 430

//Input from geometry shader
in vec4 fColor;
out vec4 fragColor;

void main()	{
	
	fragColor = fColor;

}
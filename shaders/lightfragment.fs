#version 330 core

out vec4 frag_color;

uniform vec3 lightCubeColor;

void main()
{
	frag_color = vec4(lightCubeColor, 1.0);
}	
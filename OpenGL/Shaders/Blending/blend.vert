#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 texCoord_in;

out vec2 texCoord;

uniform mat4 PVM;


void main()
{
	gl_Position = PVM * vec4(pos_in, 1.0f);
	texCoord = texCoord_in;
}
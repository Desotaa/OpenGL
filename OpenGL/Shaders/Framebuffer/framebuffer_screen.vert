#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec2 texCoord_in;

out vec2 texCoord;


void main()
{
	gl_Position = vec4(pos_in.x, pos_in.y, 0.0f, 1.0f);
	texCoord = texCoord_in;
}
#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec3 normal_in;
layout (location = 2) in vec2 texCoord_in;

out VS_OUT
{
	vec2 texCoord;
} vs_out;


void main()
{
	gl_Position = vec4(pos_in, 1.0f);
	vs_out.texCoord = texCoord_in;
}
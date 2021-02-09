#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal_in;

out vec3 fragPos;
out vec3 normal;

uniform mat4 model;
uniform mat4 PVM;
uniform mat3 normalTransformation;

void main()
{
	gl_Position = PVM * vec4(pos, 1.0f);
	fragPos = vec3(model * vec4(pos, 1.0f));
	normal = normalTransformation * normal_in;
}
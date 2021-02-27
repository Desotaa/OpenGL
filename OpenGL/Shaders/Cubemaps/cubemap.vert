#version 330 core

layout (location = 0) in vec3 pos_in;
layout (location = 1) in vec3 normal_in;

out vec3 normal;
out vec3 worldPos;

uniform mat4 model;
uniform mat4 PVM;
uniform mat3 normalTransformation;

void main()
{
	normal = normalTransformation * normal_in;
	worldPos = vec3(model * vec4(pos_in, 1.0f));
	gl_Position = PVM * vec4(pos_in, 1.0f);
}
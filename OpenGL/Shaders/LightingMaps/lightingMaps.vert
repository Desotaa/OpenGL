#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoord;


out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 PVM;
uniform mat4 model;
uniform mat3 normalTransformation;

void main()
{
	gl_Position = PVM * vec4(inPos, 1.0f);

	fragPos = vec3(model * vec4(inPos, 1.0f));
	normal = normalTransformation * inNormal;
	texCoord = inTexCoord;

}
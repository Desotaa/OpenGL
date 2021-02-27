#version 330 core
layout (location = 0) in vec3 pos_in;

out vec3 texCoord;

uniform mat4 PV;

void main()
{
	vec4 pos = PV * vec4(pos_in, 1.0f);
	//Passing z as w so that resulting depth value in NDC will always be 1.0f (max depth value of a fragment)
	gl_Position = pos.xyww;
	texCoord = pos_in;
}  
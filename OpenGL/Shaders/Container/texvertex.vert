#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 inTexCoord;

out vec3 outCol;
out vec2 texCoord;

void main()
{
	gl_Position = vec4(pos, 1.0f);
	outCol = color;
	texCoord = inTexCoord;
}

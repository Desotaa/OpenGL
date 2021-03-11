#version 330 core
layout (location = 0) in vec2 pos_in;
layout (location = 1) in vec3 color_in;

out VS_OUT
{
	vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(pos_in.x, pos_in.y, 0.0, 1.0); 
	vs_out.color = color_in;
}
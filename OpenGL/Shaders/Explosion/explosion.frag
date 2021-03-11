#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse1;
};

uniform Material material;

void main()
{
	FragColor = texture(material.texture_diffuse1, TexCoord);
}
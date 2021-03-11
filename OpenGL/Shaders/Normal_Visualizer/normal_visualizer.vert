#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT 
{
    vec3 normal;
} vs_out;

uniform mat4 modelView;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vs_out.normal = normalize(normalMatrix * aNormal);
    gl_Position = modelView * vec4(aPos, 1.0); 
}
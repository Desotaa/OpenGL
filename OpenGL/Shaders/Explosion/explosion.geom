#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in VS_OUT
{
	vec2 texCoord;
} gs_in[];


out vec2 TexCoord;

uniform float time;
uniform mat4 PVM;

vec3 getNormal()
{
	vec3 a = vec3(gl_in[1].gl_Position) - vec3(gl_in[0].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[0].gl_Position);

	return normalize(cross(a,b));
}

vec4 explode(vec4 position, vec3 normal)
{
	float magnitude = 2.0f;
	vec3 direction = ((sin(time) + 1.0f) / 2.0f) * normal * magnitude;
	return position + vec4(direction, 0.0f);
}


void main()
{
	vec3 normal = getNormal();
	//Change coordinate spaces after exploding the object in its local space
	gl_Position = PVM * explode(gl_in[0].gl_Position, normal);
	TexCoord = gs_in[0].texCoord;
	EmitVertex();
	gl_Position = PVM * explode(gl_in[1].gl_Position, normal);
	TexCoord = gs_in[1].texCoord;
	EmitVertex();
	gl_Position = PVM * explode(gl_in[2].gl_Position, normal);
	TexCoord = gs_in[2].texCoord;
	EmitVertex();
	EndPrimitive();
}
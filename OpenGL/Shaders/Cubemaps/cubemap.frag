#version 330 core

out vec4 FragColor;

in vec3 normal;
in vec3 worldPos;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
	vec3 incident = normalize(worldPos - cameraPos);

	//For Reflection
	//vec3 reflected = reflect(incident, normalize(normal));

	//For Refraction
	float ratio = 1.00 / 2.42;
	vec3 refracted = refract(incident, normalize(normal), ratio);

	FragColor = vec4(texture(skybox, refracted).rgb,1.0f);
}
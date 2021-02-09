#version 330 core

in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;

void main()
{
	//Ambient
	float ambientStrength = 0.1f;
	vec3 ambient = ambientStrength * lightColor;
	// Diffuse
	vec3 n = normalize(normal);
	vec3 fragToLight = normalize(lightPos - fragPos);
	float diffCoefficient = max(dot(n, fragToLight),0.0);
	vec3 diffuse = diffCoefficient * lightColor;
	//Specular
	vec3 fragToCamera = normalize(cameraPos - fragPos);	
	float specularStrength = 0.5f;
	vec3 reflectedDirection = reflect(-fragToLight, n);
	float specularCoefficient = pow(max(dot(reflectedDirection, fragToCamera), 0.0f), 32);
	vec3 specular = specularStrength * specularCoefficient * lightColor;

	vec3 result = (diffuse + ambient + specular) * objectColor;
	FragColor = vec4(result, 1.0f);
}
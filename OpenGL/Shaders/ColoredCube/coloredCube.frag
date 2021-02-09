#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

in vec3 normal;
in vec3 fragPos;

out vec4 FragColor;


uniform vec3 cameraPos;

uniform Material material;
uniform Light light;


void main()
{
	//Ambient
	vec3 ambient = material.ambient * light.ambient;
	// Diffuse
	vec3 n = normalize(normal);
	vec3 fragToLight = normalize(light.position - fragPos);
	float diffCoefficient = max(dot(n, fragToLight),0.0);
	vec3 diffuse = (material.diffuse * diffCoefficient) * light.diffuse;
	//Specular
	vec3 fragToCamera = normalize(cameraPos - fragPos);	
	vec3 reflectedDirection = reflect(-fragToLight, n);
	float specularCoefficient = pow(max(dot(reflectedDirection, fragToCamera), 0.0f), material.shininess);
	vec3 specular = (material.specular * specularCoefficient) * light.specular;

	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result, 1.0f);
}
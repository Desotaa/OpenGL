#version 330 core

out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;


uniform vec3 cameraPos;
uniform Material material;
uniform Light light;

void main()
{
	//Ambient
	vec3 ambient = light.ambient * vec3((texture(material.diffuse, texCoord)));

	//Diffuse
	vec3 n = normalize(normal);
	vec3 fragToLight = normalize(light.position - fragPos);
	float diffCoeff = max(dot(fragToLight, n), 0.0f);
	vec3 diffuse = light.diffuse * diffCoeff * vec3(texture(material.diffuse, texCoord));

	//Specular
	vec3 fragToCamera = normalize(cameraPos - fragPos);
	vec3 reflectDir = reflect(-fragToLight, n);
	float specCoeff = pow(max(dot(reflectDir, fragToCamera), 0.0f), material.shininess);
	vec3 specular = light.specular * specCoeff * vec3(texture(material.specular, texCoord));

	vec3 result = ambient + diffuse + specular;

	FragColor = vec4(result, 1.0f);

}
#version 330 core

out vec4 FragColor;

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};


in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;


uniform vec3 cameraPos;
uniform Material material;
uniform DirLight dirLight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

vec3 calculateDirLight(DirLight light, vec3 normal, vec3 fragToCamera)
{
	vec3 fragToLight = normalize(-light.direction);
	//Diffuse shading
	float diffCoeff = max(dot(fragToLight, normal), 0.0f);
	//Specular shading
	vec3 reflected = reflect(-fragToLight, normal);
	float specularCoeff = pow(max(dot(reflected, fragToCamera), 0.0f), material.shininess);
	//Combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
	vec3 diffuse = light.diffuse * diffCoeff * vec3(texture(material.diffuse, texCoord));
	vec3 specular = light.specular * specularCoeff * vec3(texture(material.specular, texCoord));

	return (ambient + diffuse + specular);
}

vec3 calculatePointLight(PointLight light, vec3 normal, vec3 fragPosition, vec3 fragToCamera)
{
	vec3 fragToLight = normalize(light.position - fragPosition);
	//Diffuse shading
	float diffCoeff = max(dot(fragToLight, normal), 0.0f);
	//Specular shading
	vec3 reflected = reflect(-fragToLight, normal);
	float specularCoeff = pow(max(dot(reflected, fragToCamera), 0.0f), material.shininess);

	//Attenuation
	float dist = length(light.position - fragPosition);
	float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	//Combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
	vec3 diffuse = light.diffuse * diffCoeff * vec3(texture(material.diffuse, texCoord));
	vec3 specular = light.specular * specularCoeff * vec3(texture(material.specular, texCoord));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);

}

vec3 calculateSpotLight(SpotLight light, vec3 normal, vec3 fragPosition, vec3 fragToCamera)
{
	vec3 fragToLight = normalize(light.position - fragPosition);
	//Diffuse shading
	float diffCoeff = max(dot(fragToLight, normal), 0.0f);
	//Specular shading
	vec3 reflected = reflect(-fragToLight, normal);
	float specularCoeff = pow(max(dot(reflected, fragToCamera), 0.0f), material.shininess);

	//Attenuation
	float dist = length(light.position - fragPosition);
	float attenuation = 1.0f / (light.constant + light.linear * dist + light.quadratic * dist * dist);

	//Spotlight intensity
	float theta = dot(fragToLight, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float alpha = (theta - light.outerCutOff) / epsilon;
	float intensity = smoothstep(0.0f, 1.0f, alpha);

	//Combine the results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));
	vec3 diffuse = light.diffuse * diffCoeff * vec3(texture(material.diffuse, texCoord));
	vec3 specular = light.specular * specularCoeff * vec3(texture(material.specular, texCoord));
	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);
}

void main()
{
	//Fundamental variables
	vec3 n = normalize(normal);
	vec3 fragToCamera = normalize(cameraPos - fragPos);

	vec3 result = calculateDirLight(dirLight, n, fragToCamera);

	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
		result += calculatePointLight(pointLights[i], n, fragPos, fragToCamera);

	result += calculateSpotLight(spotLight, n, fragPos, fragToCamera);

	FragColor = vec4(result, 1.0f);

}
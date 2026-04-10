#version 330 core

struct materials
{
  float shininess;

  sampler2D specular;		
  sampler2D diffuse;
  
};
struct Light
{
  vec3 position;
  vec3 direction;
  float innerCone;
  float outerCone;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

struct dirLight
{
   vec3 direction;	
}

out vec4 frag_color;

uniform Light light;
uniform materials material;
uniform vec3 viewPos;
uniform bool flash;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

void calculateDirectionalLight();
void calculatePointLight()
{
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;

   vec3 norm = normalize(normal);
   vec3 light_direction = normalize(-dirLight.direction);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

   vec3 view_direction = normalize(viewPos - frag_pos);
   vec3 reflect_direction = reflect(-lightDir, norm);
   float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
   vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));
 
}

void main()
{
	vec3 lightDir = normalize(light.position - frag_pos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.innerCone - light.outerCone);
	float intensity = clamp((theta - light.outerCone) / epsilon, 0.0, 1.0);

	if (theta > epsilon && flash == true)
	{

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(viewPos - frag_pos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

	vec3 view_direction = normalize(viewPos - frag_pos);
	vec3 reflect_direction = reflect(-lightDir, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

	float distance = length(light.position - frag_pos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	diffuse *= intensity;
	specular *= intensity;
	
	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
	
	}
	else
	{
	frag_color = vec4(light.ambient * vec3(texture(material.diffuse, tex_coords)), 1.0);
	}

	
}


void calculateDirectionalLight()
{
   vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;

   vec3 norm = normalize(normal);
   vec3 light_direction = normalize(-dirLight.direction);
   float diff = max(dot(norm, lightDir), 0.0);
   vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

   vec3 view_direction = normalize(viewPos - frag_pos);
   vec3 reflect_direction = reflect(-lightDir, norm);
   float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
   vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));
 
}
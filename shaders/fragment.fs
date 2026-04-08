#version 330 core

struct materials
{
  float shininess;

  sampler2D specular;		
  sampler2D diffuse;
  
};
struct Light
{

  vec3 lightDirection;
  vec3 spotDirection;
  float spotCutoff;
  float spotTheta;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;

  float constant;
  float linear;
  float quadratic;
};

out vec4 frag_color;

uniform Light light;
uniform materials material;
uniform vec3 viewPos;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

void main()
{


	float distance = length(view - frag_pos);
	float attenuation = 1.0 / (light.constant  + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(lightDirection);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

	vec3 view_direction = normalize(view_pos - frag_pos);
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));

	light.lightDirection = viewPos - frag_pos;
	light.spotDirection = view_direction;
	light.spotCutoff = 90.0;
	light.spotTheta = cos(dot(light.lightDirection, spotDirection));


	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
	
}
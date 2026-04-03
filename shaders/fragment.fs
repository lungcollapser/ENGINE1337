#version 330 core

struct materials
{
  float shininess;
  vec2 scale;
  vec3 diffuseColor;

  sampler2D specular;		
  sampler2D diffuse;
  sampler2D emission;
  
};
struct Light
{
  vec3 position;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 emission;
};

out vec4 frag_color;

uniform Light light;
uniform materials material;
uniform vec3 light_pos;
uniform vec3 view_pos;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

void main()
{

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

	vec3 view_direction = normalize(view_pos - frag_pos);
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords)) * material.diffuseColor;

	vec3 emission = vec3(texture(material.emission, tex_coords)).rgb;	     

	vec3 result = ambient + diffuse + specular + emission;
	frag_color = vec4(result, 1.0);
	
}
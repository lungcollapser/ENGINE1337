#version 330 core

struct materials
{
  float shininess;

  sampler2D specular;		
  sampler2D diffuse;
  
};
struct Light
{
  vec3 direction;

  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 frag_color;

uniform Light light;
uniform materials material;
uniform vec3 view_pos;

in vec3 normal;
in vec3 frag_pos;
in vec2 tex_coords;

void main()
{

	vec3 ambient = light.ambient * vec3(texture(material.diffuse, tex_coords)).rgb;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(-light.direction);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, tex_coords)).rgb;

	vec3 view_direction = normalize(view_pos - frag_pos);
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, tex_coords));


	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
	
}
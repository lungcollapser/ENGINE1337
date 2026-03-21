#version 330 core

struct materials
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

out vec4 frag_color;

uniform Light light;
uniform materials material;
uniform vec3 light_pos;
uniform vec3 view_pos;
uniform mat4 light_rotation;

in vec3 normal;
in vec3 frag_pos;

void main()
{
	
	vec3 ambient = light.ambient * material.ambient;
	
	vec3 norm = normalize(normal);
	vec3 light_direction = normalize(light_pos - frag_pos);
	float diff = max(dot(norm, light_direction), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	vec3 view_direction = normalize(view_pos - frag_pos);
	vec3 reflect_direction = reflect(-light_direction, norm);
	float spec = pow(max(dot(view_direction, reflect_direction), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	vec3 result = ambient + diffuse + specular;
	frag_color = vec4(result, 1.0);
	
}
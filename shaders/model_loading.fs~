#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

struct pointLight
{
   vec3 position;


   vec3 ambient;
   vec3 diffuse;
   vec3 specular;


   float constant;
   float linear;
   float quadratic;

   vec3 lightColor;
};

uniform pointLight point;
uniform vec3 viewPos;
uniform float shininess;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

vec3 calculatePointLight(pointLight point, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    vec3 result = calculatePointLight(point, norm, viewDir);

    FragColor = vec4(result, 1.0);
}
vec3 calculatePointLight(pointLight point, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(point.position - fragPos);

   float diff = max(dot(normal, lightDir), 0.0);
   vec3 reflectDir = reflect(-lightDir, normal);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

   float distance = length(point.position - fragPos);
   float attenuation = 1.0 / (point.constant + point.linear * distance + point.quadratic * (distance * distance));

   vec3 ambient = point.ambient * vec3(texture(texture_diffuse1, texCoords));
   vec3 diffuse = point.diffuse * diff * vec3(texture(texture_diffuse1, texCoords));
   vec3 specular = point.specular * spec * vec3(texture(texture_specular1, texCoords));

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse + specular);
};


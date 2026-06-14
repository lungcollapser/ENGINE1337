#version 330 core

#define NR_POINT_LIGHTS 4

struct materials
{
  float shininess;

  sampler2D diffuse;
  sampler2D specular;		  
};
struct dirLight
{
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};
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
struct spotLight
{
   vec3 position;
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;

   float innerCone;
   float outerCone;
};

out vec4 frag_color;

uniform materials material;
uniform dirLight directional;
uniform pointLight point[NR_POINT_LIGHTS];
uniform spotLight spot;
uniform vec3 viewPos;
uniform bool flash;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

vec3 calculateDirectionalLight(dirLight dir, vec3 normal, vec3 viewDir);
vec3 calculatePointLight(pointLight point, vec3 normal, vec3 viewDir); 
vec3 calculateSpotLight(spotLight spot, vec3 normal, vec3 viewDir); 

void main()
{
   vec3 norm = normalize(normal);
   vec3 viewDir = normalize(viewPos - fragPos);
   
   vec3 result = calculateDirectionalLight(directional, norm, viewDir);

   for (int i = 0; i < NR_POINT_LIGHTS; i++)
   {
   
      result += calculatePointLight(point[i], norm, viewDir);
   }
   result += calculateSpotLight(spot, norm, viewDir);

   frag_color = vec4(result, 1.0);
}


vec3 calculateDirectionalLight(dirLight dir, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(-dir.direction);

   float diff = max(dot(normal, lightDir), 0.0);
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

   vec3 ambient = dir.ambient * vec3(texture(material.diffuse, texCoords));
   vec3 diffuse = dir.diffuse * diff * vec3(texture(material.diffuse, texCoords));
   vec3 specular = dir.specular * spec * vec3(texture(material.specular, texCoords));
   return (ambient + diffuse + specular);
};
vec3 calculatePointLight(pointLight point, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(point.position - fragPos);

   float diff = max(dot(normal, lightDir), 0.0);
   vec3 reflectDir = reflect(-lightDir, normal);

   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

   float distance = length(point.position - fragPos);
   float attenuation = 1.0 / (point.constant + point.linear * distance + point.quadratic * (distance * distance));

   vec3 ambient = point.ambient * vec3(texture(material.diffuse, texCoords)) * point.lightColor;
   vec3 diffuse = point.diffuse * diff * vec3(texture(material.diffuse, texCoords));
   vec3 specular = point.specular * spec * vec3(texture(material.specular, texCoords));

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;

   return (ambient + diffuse + specular);
};
vec3 calculateSpotLight(spotLight spot, vec3 normal, vec3 viewDir)
{
   vec3 lightDir = normalize(spot.position - fragPos);
   float theta = dot(lightDir, normalize(-spot.direction));
   float epsilon = (spot.innerCone - spot.outerCone);
   float intensity = clamp((theta - spot.outerCone) / epsilon, 0.0, 1.0);

   if (theta > epsilon)
   {
      
   float diff = max(dot(normal, lightDir), 0.0);
   vec3 reflectDir = reflect(-lightDir, normal);
   float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   
   vec3 ambient = spot.ambient * vec3(texture(material.diffuse, texCoords));
   vec3 diffuse = spot.diffuse * diff * vec3(texture(material.diffuse, texCoords));
   vec3 specular = spot.specular * spec * vec3(texture(material.specular, texCoords));

   float distance = length(spot.position - fragPos);
   float attenuation = 1.0 / (spot.constant + spot.linear * distance + spot.quadratic * (distance * distance));

   ambient *= attenuation * intensity;
   diffuse *= attenuation * intensity;
   specular *= attenuation * intensity;

   return (ambient + diffuse + specular);
   }
   else
   {
   return spot.ambient * vec3(texture(material.diffuse, texCoords));
   }
}

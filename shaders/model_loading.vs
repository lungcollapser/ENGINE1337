#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoords = vec2(aTexCoords.x, 1.0 - aTexCoords.y);
    normal = mat3(transpose(inverse(model))) * aNormal;	
    gl_Position = projection * view * vec4(fragPos, 1.0);	
}
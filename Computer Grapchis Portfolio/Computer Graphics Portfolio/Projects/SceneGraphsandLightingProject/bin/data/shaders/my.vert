#version 410

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;

out vec3 fragNormal;
out vec3 worldPosition;
out vec3 fragPosition;
uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 mvp; 

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    
    worldPosition = (model * vec4(position, 1.0)).xyz;
    fragNormal = normalize(normalMatrix * normal);
}


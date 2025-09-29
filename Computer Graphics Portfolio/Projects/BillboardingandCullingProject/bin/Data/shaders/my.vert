#version 410

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 meshColor;

out vec2 fragUV;
out vec2 fragUV2;
out vec3 fragNormal;

uniform vec2 uvScale;
uniform vec2 uvStart;
uniform vec2 uvStart2;

uniform mat4 mvp;
uniform mat3 normalMatrix;
uniform vec3 translation;

void main()
{
    gl_Position = mvp * vec4(position+translation, 1.0);


    fragNormal = normalMatrix * normal;

      
   fragUV = vec2(uv.x, 1.0 - uv.y) * uvScale + uvStart;


   fragUV2 = vec2(uv.x, 1.0 - uv.y) * uvScale + uvStart2;
}

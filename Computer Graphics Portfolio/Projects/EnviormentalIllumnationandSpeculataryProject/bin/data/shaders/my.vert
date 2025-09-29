#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 tangent;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;
out vec2 fragUV;
out mat3 TBN;
out vec3 fragNormal;
out vec3 worldPosition;
out vec3 fragPosition;
uniform mat3 normalMatrix;
uniform mat4 model;
uniform mat4 mvp; 

void main()
{
    gl_Position = mvp * vec4(position, 1.0);
    fragUV = vec2(uv.x, 1 - uv.y);
    //worldPosition = (model * vec4(position, 1.0)).xyz;
    //fragNormal = normalize(normalMatrix * normal);
    vec3 T = normalize(normalMatrix *tangent.xyz);
    vec3 B=  normalize(normalMatrix * cross(tangent.xyz, normal));
    vec3 N = normalize(normalMatrix * normal);
    TBN =mat3(T,B,N);
    worldPosition = (model * vec4(position, 1.0)).xyz;
}

//
//#version 410
//
//layout (location = 0) in vec3 position;
//uniform mat4 mvp;
//out vec3 fragPos;
//void main(){
//	fragPos = position;
//	gl_Position = mvp * vec4(position, 1.0);
//}
//
//
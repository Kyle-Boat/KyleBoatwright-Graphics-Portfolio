#version 330

layout (location = 0) in vec3 position;
layout (location = 2) in vec3 normal;
uniform mat4 model;
uniform mat4 MV;
uniform mat4 mvp;
out vec3 fragNormal;
out vec3 CamPosition;

void main()
{
	CamPosition = vec3(MV[3]);
	gl_Position = mvp* vec4(position, 1.0);
	fragNormal = normalize(mat3(model) * normal);
}
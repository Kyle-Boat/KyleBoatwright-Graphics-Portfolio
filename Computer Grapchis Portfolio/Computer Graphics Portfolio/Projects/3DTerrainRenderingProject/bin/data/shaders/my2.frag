#version 330

in vec3 fragNormal;
in vec3 fragPos;
out vec4 outColor;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 meshColor;

void main()
{
	vec3 normal = normalize(fragNormal);
	float nDotL=max(0, dot(normal,lightDirection));
	vec3 irradiance = lightColor * nDotL;
	outColor = vec4(meshColor * irradiance,1.0);

}


 
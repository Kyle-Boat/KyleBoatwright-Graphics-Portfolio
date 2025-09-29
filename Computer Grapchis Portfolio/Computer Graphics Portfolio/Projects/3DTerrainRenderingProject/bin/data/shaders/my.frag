#version 330

in vec3 fragNormal;
in vec3 fragPos;
out vec4 outColor;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 meshColor;
uniform vec3 CamPosition;

void main()
{
//
//float dist = distance(CamPosition,vec3(0,0,0));
//float opacity = smoothstep(10,5,dist);

	float distance = length(CamPosition - fragPos);
    float alpha = clamp(1.0 - (distance-350) / 550.0, 0.0, 1.0); // Adjust the divisor to control the fade distance

    //vec3 color = meshColor * lightColor * max(dot(normalize(fragPos), lightDirection), 0.0);

	vec3 normal = normalize(fragNormal);
	float nDotL=max(0, dot(normal,lightDirection));
	vec3 irradiance = lightColor * nDotL;
	outColor = vec4(meshColor * irradiance,alpha);

}


 
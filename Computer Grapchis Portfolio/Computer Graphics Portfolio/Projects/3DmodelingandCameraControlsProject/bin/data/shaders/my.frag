#version 330

in vec3 fragNormal;
in vec3 CamPosition;
out vec4 outColor;


void main()
{

	float dist = distance(CamPosition,vec3(0,0,0));
	float opacity = smoothstep(10,5,dist);
	outColor = vec4(normalize(fragNormal) *0.5 + 0.5,opacity);

}


 
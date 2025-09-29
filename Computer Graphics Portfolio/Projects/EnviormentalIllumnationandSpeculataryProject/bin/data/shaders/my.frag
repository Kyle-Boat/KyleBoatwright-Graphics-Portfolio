#version 330

in mat3 TBN;
in vec3 fragPos;
in vec2 fragUV;
out vec4 outColor;
//uniform sampler2D tex;
uniform sampler2D diffuseTex;
uniform sampler2D normalTex;
uniform sampler2D specularTex;
uniform vec3 lightDir;
uniform vec3 lightColor;
//uniform vec3 meshColor;
uniform vec3 CamPosition;
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform samplerCube envMap;

in vec3 worldPosition;

void main()
{
	vec3 diffuseColor= pow(texture(diffuseTex,fragUV).rgb,vec3 (2.2));
	vec3 tsNormal = texture(normalTex, fragUV).xyz * 2 -1;
	vec3 wsNormal = normalize(TBN* tsNormal);
	float nDotL=max(0, dot(wsNormal,lightDir));
	
	
	vec3 specularColor= pow(texture(diffuseTex,fragUV).rgb,vec3 (2.2));


	vec3 pointLightDir = normalize(pointLightPos-worldPosition);
    float pointNDotL = max(0, dot(wsNormal, pointLightDir));
    vec3 pointLight = pointLightColor*pointNDotL;

	vec3 envIrradiance = pow(textureLod(envMap, wsNormal,99).rgb, vec3(2.2));

	vec3 irradiance = (lightColor * nDotL)+pointLight+envIrradiance;//last is ambient lighht

	vec3 view = normalize(CamPosition - worldPosition);
	vec3 envLightDir = reflect(-view, wsNormal);
	vec3 ambientDiffuse = diffuseColor * irradiance;

	vec3 envReflection = pow(texture(envMap, envLightDir).rgb, vec3(2.2));//very shiny because of this

	float envNDotL = max(0, dot(wsNormal, envLightDir));//change the floor to 0.5 to stop it becoming all white when you get close. Was 0 in video

	vec3 fresnel = mix(specularColor, vec3(1.0), pow(1-envNDotL,5));

	vec3 specularReflection = fresnel*envReflection;

	//outColor= vec4(pow(diffuseColor.rgb * irradiance, vec3(1.0/2.2)), 1);
	outColor= vec4(pow(ambientDiffuse + specularReflection, vec3(1.0/2.2)), 1);
	//vec4(texture(normalTex, fragUV).xyz,1);//diffuseColor; //vec4(fragUV,0,1);
}


 
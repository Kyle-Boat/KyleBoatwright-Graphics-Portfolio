#version 410

//in vec2 fragUV;
in vec3 fragNormal;
in vec3 worldPosition;

out vec4 outColor;

uniform vec3 dirLightDir;
uniform vec3 dirLightColor;
uniform vec3 meshColor;
uniform vec3 ambientColor;
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform vec3 spotLightPos;
uniform vec3 spotLightConeDir;
uniform vec3 spotLightColor;
uniform float spotLightCutoff;

void main()
{
    vec3 normal = normalize(fragNormal);

    // Calculate directional lighting
    float dirNDotL = max(0, dot(normal, dirLightDir));
    vec3 directionalLight = dirLightColor * dirNDotL;
    // Calculate point lighting
    vec3 pointLightDir = normalize(pointLightPos-worldPosition);
    float pointNDotL = max(0, dot(normal, pointLightDir));
    vec3 pointLight = pointLightColor*pointNDotL;

    vec3 toSpotLight = spotLightPos - worldPosition;
    vec3 spotLightDir = normalize(toSpotLight);

    float spotNDotL = max(0, dot(normal, spotLightDir));
    vec3 spotLight = spotLightColor * spotNDotL;

    // how much light the surface effectively receives
    vec3 irradiance = ambientColor + directionalLight + spotLight + pointLight;
    
    // how much light the surface reflects (physically)
    vec3 linearColor = meshColor * irradiance;

    
    // gamma encoding
    outColor = vec4(pow(linearColor, vec3(1.0 / 2.2)), 1.0);
}


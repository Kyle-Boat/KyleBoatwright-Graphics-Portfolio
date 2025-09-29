#version 410

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 meshColor;
uniform vec3 ambientColor;

in vec3 fragNormal;
out vec4 outColor;

void main()
{
    vec3 normal = normalize(fragNormal);

    float nDotL = max(0.0, dot(normal, lightDir)); // assuming lightDir pre-normalized

    // how much light is effectively received by the surface
    vec3 irradiance = ambientColor + lightColor * nDotL; 

    // Gamma correction: physically-linear to perceptually-linear (encoding gamma)
    outColor = vec4(pow(meshColor * irradiance, vec3(1.0 / 2.2)), 1.0);
}

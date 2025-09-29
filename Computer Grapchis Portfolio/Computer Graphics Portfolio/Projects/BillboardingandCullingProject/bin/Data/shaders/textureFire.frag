#version 410

uniform sampler2D tex;

in vec2 fragUV;
in vec2 fragUV2;
out vec4 outColor;

uniform float blendParam;

void main()
{
    outColor =  mix(
        texture(tex, fragUV), //first texture
        texture(tex, fragUV2), //second texture
        blendParam);
}
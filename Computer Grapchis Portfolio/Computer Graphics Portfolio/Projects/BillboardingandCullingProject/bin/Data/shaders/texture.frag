#version 410

uniform sampler2D tex;
uniform float opacityMod;

in vec2 fragUV;
out vec4 outColor;

void main()
{
    vec4 texColor = texture(tex, fragUV);
    outColor = vec4(texColor.rgb, texColor.a * opacityMod);
}

#version 410


in vec2 fragUV;
out vec4 outColor;
uniform sampler2D tex;

void main()
{
	outColor = texture(tex, fragUV);
}
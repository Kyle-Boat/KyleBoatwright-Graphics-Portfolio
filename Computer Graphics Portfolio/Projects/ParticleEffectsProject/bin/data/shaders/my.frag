#version 410

in vec4 fragColor;
in vec2 fragUV;

out vec4 outColor;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform int lightning;
uniform float fragMod;
out vec4 color ;

void main(){
	vec4 opacityMod;
	vec4 colors;
	opacityMod = vec4(fragMod,fragMod,fragMod,1.0)-.5;//additive?
	outColor = dot(texture(tex, fragUV),opacityMod)*vec4(0.72, 0.97, 0.96, 1.0);
	
	//outColor = texture(tex, fragUV)*opacityMod;
	
	
}
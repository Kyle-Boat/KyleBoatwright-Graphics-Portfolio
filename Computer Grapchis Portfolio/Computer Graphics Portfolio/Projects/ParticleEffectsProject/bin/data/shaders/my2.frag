#version 410

in vec4 fragColor;
in vec2 fragUV;

out vec4 outColor;
uniform sampler2D tex;
uniform sampler2D tex2;
uniform float fragMod;

void main(){
	
	vec4 opacityMod = vec4(1.0,1.0,1.0,fragMod);//alpha?
	outColor = texture(tex, fragUV)*opacityMod*vec4(0.47, 0.49, 0.49, 1.0);
	
	
	
	
}
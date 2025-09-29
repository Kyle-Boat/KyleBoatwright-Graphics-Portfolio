#version 410

layout (location=0) in vec3 position;
layout (location=1) in vec4 color ;
layout(location = 3) in vec2 uv;

out vec4 fragColor;
out vec2 fragUV;
uniform vec3 translation;
uniform float fragMod;

void main(){
	gl_Position = vec4(position+translation,1.0);
	
	fragColor = color;
	fragUV = uv;
}
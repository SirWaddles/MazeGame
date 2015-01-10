#version 330
out vec4 outputColor;

in vec2 uvs;
in vec4 gColour;

uniform sampler2D baseImage;

void main(){
	outputColor = texture(baseImage, uvs) * gColour;
}
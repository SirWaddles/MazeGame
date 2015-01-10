#version 330

in vec4 theColor;
in vec2 theUVs;

out vec4 outputColor;
uniform sampler2D baseImage;
void main()
{
	vec4 texCol = texture(baseImage, theUVs);
	outputColor = texCol*theColor;
}

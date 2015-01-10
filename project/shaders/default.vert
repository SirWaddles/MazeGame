#version 330
layout(location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout(location = 2) in vec2 uvs;

out vec4 theColor;
out vec2 theUVs;

uniform vec2 scrSize;
void main()
{
	vec4 fin = position;
	fin.x = (position.x / scrSize.x) * 2.0f - 1.0f;
	fin.y = (position.y / scrSize.y) * 2.0f - 1.0f;
	fin.y = fin.y * -1.0f;
	gl_Position = fin;
	theColor = color;
	theUVs = uvs;
}
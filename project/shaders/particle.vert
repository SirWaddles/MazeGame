#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colour;
layout(location = 2) in float size;

uniform mat4 modelView;

out NewVertex{
	vec4 camPos;
	vec4 fColour;
	float pSize;
};

void main(){
	vec4 tPos = modelView * vec4(position, 1.0f);
	camPos = tPos;
	fColour = colour;
	pSize = size;
	//gl_Position = camPos;
}
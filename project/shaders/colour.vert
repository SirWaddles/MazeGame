#version 330
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normal;

uniform mat4 projMat;
uniform mat4 modelView;
uniform mat3 nModelView;

out vec3 vertexNormal;
out vec2 uvcs;
out vec3 cameraSpacePosition;


void main()
{
	vec4 tCam = modelView * vec4(position, 1.0);
	gl_Position = projMat * tCam;
	
	vertexNormal = nModelView * normal;
	//vertexNormal = normal;
	cameraSpacePosition = vec3(tCam);
	
	//vec2 nUV = uvs;
	//nUV.y = 1.0f - nUV.y;
	uvcs = uvs;
}


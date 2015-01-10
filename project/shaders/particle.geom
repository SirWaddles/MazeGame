#version 330
layout(points) in;
layout(triangle_strip, max_vertices = 6) out;

in NewVertex {
	vec4 camPos;
	vec4 fColour;
	float pSize;
} nVerts[1];

out vec2 uvs;
out vec4 gColour;

uniform mat4 projMat;

void main(){
	gColour = nVerts[0].fColour;
	
	vec4 nPos1 = nVerts[0].camPos;
	
	vec4 nPos2 = nPos1;
	nPos2.x = nPos2.x + nVerts[0].pSize;
	
	vec4 nPos3 = nPos1;
	nPos3.x = nPos3.x + nVerts[0].pSize;
	nPos3.y = nPos3.y + nVerts[0].pSize;
	
	vec4 nPos4 = nPos1;
	nPos4.y = nPos4.y + nVerts[0].pSize;
	
	nPos1 = projMat * nPos1;
	nPos2 = projMat * nPos2;
	nPos3 = projMat * nPos3;
	nPos4 = projMat * nPos4;
	
	uvs = vec2(0.0f, 0.0f);
	gl_Position = nPos1;
	EmitVertex();
	
	uvs = vec2(0.0f, 1.0f);
	gl_Position = nPos2;
	EmitVertex();
	
	uvs = vec2(1.0f, 1.0f);
	gl_Position = nPos3;
	EmitVertex();
	EndPrimitive();
	
	uvs = vec2(0.0f, 0.0f);
	gl_Position = nPos1;
	EmitVertex();
	
	uvs = vec2(1.0f, 1.0f);
	gl_Position = nPos3;
	EmitVertex();
	
	uvs = vec2(1.0f, 0.0f);
	gl_Position = nPos4;
	EmitVertex();
	EndPrimitive();
	
}
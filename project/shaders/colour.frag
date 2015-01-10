#version 330

out vec4 outputColor;

in vec3 cameraSpacePosition;
in vec2 uvcs;
in vec3 vertexNormal;

const int max_lights = 16;

uniform vec4 lightIntensity[max_lights];
uniform vec3 cameraSpaceLightPos[max_lights];
uniform float lightAttenuation[max_lights];
uniform float shininessFactor[max_lights];

uniform vec4 specularColor;
uniform vec4 diffuseColor;
uniform vec4 ambientColor;

uniform sampler2D baseImage;

/*float CalcAttenuation(vec3 cSpacePos, out vec3 lightDirection, int nDex)
{
	vec3 lightDifference =  cameraSpaceLightPos[nDex] - cSpacePos;
	float lightDistanceSqr = dot(lightDifference, lightDifference);
	lightDirection = lightDifference * inversesqrt(lightDistanceSqr);
	
	return (1 / ( 1.0 + lightAttenuation[nDex] * sqrt(lightDistanceSqr)));
	//return sqrt(lightDistanceSqr);
}*/

vec4 CalcLight(int nDex, vec3 surfaceNormal, vec3 viewDirection){
	vec3 lDiff = cameraSpaceLightPos[nDex] - cameraSpacePosition;
	float lDist2 = dot(lDiff, lDiff);
	float atten = 1.0f-clamp(sqrt(lDist2)/lightAttenuation[nDex],0,1);
	vec3 lightDir = lDiff * inversesqrt(lDist2);
	float cAng = clamp(dot(lightDir, surfaceNormal), 0, 1);
	return vec4(vec3(atten*cAng), 1.0f) * lightIntensity[nDex];
	//return vec4(atten, atten, atten, 1.0f);
}

void main()
{
	vec3 sNormal = normalize(vertexNormal);
	vec3 vDirection = normalize(-cameraSpacePosition);
	
	vec4 fColor;
	fColor=(diffuseColor * ambientColor);
	for(int i=0; i<max_lights; i++){
		fColor += CalcLight(i, sNormal, vDirection);
	}
	vec4 texCol = texture(baseImage, uvcs);
	outputColor = fColor*texCol;
	
}

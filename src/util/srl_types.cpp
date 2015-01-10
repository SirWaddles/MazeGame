#include "util/srl_types.h"

std::string SrlT::GetFloat(float x){
	char fVal[4];
	*(float*)fVal = x;
	return std::string(fVal, 4);
}

float SrlT::SetFloat(std::string x){
	float y;
	char* t = reinterpret_cast<char*>(&y);
	*t=x[0];
	*(t+1)=x[1];
	*(t+2)=x[2];
	*(t+3)=x[3];
	return y;
}

std::string SrlT::GetInt(unsigned int x){
	char fVal[4];
	*(unsigned int*)fVal = x;
	return std::string(fVal, 4);
}

unsigned int SrlT::SetInt(std::string x){
	unsigned int y;
	char* t = reinterpret_cast<char*>(&y);
	*t=x[0];
	*(t+1)=x[1];
	*(t+2)=x[2];
	*(t+3)=x[3];
	return y;
}

std::string SrlT::GetShort(unsigned short x){
	char fVal[2];
	*(unsigned short*)fVal = x;
	return std::string(fVal, 2);
}

unsigned short SrlT::SetShort(std::string x){
	unsigned short y;
	char* t = reinterpret_cast<char*>(&y);
	*t=x[0];
	*(t+1)=x[1];
	return y;
}
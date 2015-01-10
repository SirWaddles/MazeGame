#ifndef _SRL_TYPES_HEADER_
#define _SRL_TYPES_HEADER_

#include <string>

namespace SrlT {
	
	std::string GetFloat(float x);
	std::string GetInt(unsigned int x);
	std::string GetShort(unsigned short x);

	float SetFloat(std::string x);
	unsigned int SetInt(std::string x);
	unsigned short SetShort(std::string x);


}



#endif //_SRL_TYPES_HEADER
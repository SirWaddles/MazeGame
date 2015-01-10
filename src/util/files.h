#ifndef _FILE_HEADER_
#define _FILE_HEADER_

#include <vector>
#include <string>

namespace Util{
	std::vector<std::string> GetDirectoryContents(char* path);
	std::string LoadFile(const char* location);
}

#endif
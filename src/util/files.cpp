#include "files.h"

#include <fstream>
#include <streambuf>

using namespace std;

namespace Util{

	string LoadFile(const char* location){
		ifstream t(location);
		string done((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		return done;
	}

}
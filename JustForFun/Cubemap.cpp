#include "Cubemap.h"

#include <sstream>

std::string JFF::Cubemap::generateCacheName(const char* assetFilepath)
{
	std::ostringstream ss;
	ss << "Cubemap://";
	ss << assetFilepath;

	return ss.str();
}

#include "Image.h"

#include <sstream>

std::string JFF::Image::generateCacheName(const char* filepath)
{
	std::ostringstream ss;
	ss << "Image://";
	ss << filepath;

	return ss.str();
}

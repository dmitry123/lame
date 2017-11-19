#include "FileLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <fstream>

LAME_LANG_BEGIN

SInt8P FileLoader::load() {

	std::streampos begin,
		end;

	std::ifstream stream(this->getName());

	begin = stream.tellg();
	stream.seekg(0, std::ios::end);
	end = stream.tellg();
	stream.seekg(0, std::ios::beg);

	std::streamsize size = std::streamsize(end - begin);
	SInt8P buffer = new SInt8[ UInt32(size) + 1 ]{ 0 };

	stream.read(buffer, size);
	stream.close();

	return buffer;
}

LAME_LANG_END
#include "FileParser.h"

LAME_BEGIN2(VirtualMachine)

FileParser::FileParser(Void) {

}

FileParser::~FileParser(Void) {

}

Void FileParser::LoadFile(StringC fileName) {

	Core::File file;

	file.Open(fileName, "rt");
}

LAME_END2

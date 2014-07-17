#include "Buffer.h"

LAME_BEGIN2(Core)

Void Buffer::Format(StringC format, ...) {

	int size = 512;
	char* buffer = new char[size];

	va_list vl;
	va_start(vl, format);

	int nsize = vsnprintf(buffer, size, format, vl);

	if (size <= nsize) {

		delete[] buffer;

		buffer = 0;
		buffer = new char[nsize + 1];

		vsnprintf(buffer, size, format, vl);
	}

	*this = buffer;

	va_end(vl);

	delete[] buffer;
}

List <Buffer> Buffer::Split(Sint8 symbol) const {

	List <Buffer> list;
	StringC first;
	StringC where;
	Uint32 last;
	Sint8P buffer;

	buffer = (Sint8P)this->data();
	first = this->data();
	last = 0;

	while ((first = strchr(first + 1, symbol))) {

		buffer[first - buffer] = '\0';

		where = buffer + last;

		while (*where == symbol) {
			++where;
		}

		if (*where) {
			list.push_back(where);
		}

		last += list.back().length();

		buffer[first - buffer] = symbol;

		while (*first == symbol) {
			++first;
			++last;
		}
	}

	where = buffer + last;

	while (*where == symbol) {
		++where;
	}

	if (*where) {
		list.push_back(where);
	}

	return list;
}

Void Buffer::Get(Void) {

	Sint32 symbol;
	Buffer string;

	while (LAME_TRUE) {

		symbol = getchar();

		if (symbol == '\n' || symbol == '\r') {
			break;
		}

		if (symbol == '\b' && string.length() > 0) {
			string.pop_back();
		}

		string += symbol;
	}

	*this = string;
}

Uint16 Buffer::GetHash16(Void) const {
	return GetHash16(this->data());
}

Uint32 Buffer::GetHash32(Void) const {
	return GetHash32(this->data());
}

Uint64 Buffer::GetHash64(Void) const {
	return GetHash64(this->data());
}

Uint16 Buffer::GetHash16(StringC string) {

	Uint32 length = 0;
	Uint16 hash = 0;

	length = strlen(string);

	for (Uint32 i = 0; i < length; i++) {
		hash = 15 * hash + string[i];
	}
	return hash;
}

Uint32 Buffer::GetHash32(StringC string) {

	Uint32 length = 0;
	Uint32 hash = 0;

	length = strlen(string);

	for (Uint32 i = 0; i < length; i++) {
		hash = 31 * hash + string[i];
	}
	return hash;
}

Uint64 Buffer::GetHash64(StringC string) {

	Uint32 length = 0;
	Uint64 hash = 0;

	length = strlen(string);

	for (Uint32 i = 0; i < length; i++) {
		hash = 63 * hash + string[i];
	}
	return hash;
}

LAME_END2
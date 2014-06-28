#include "Utility.h"

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

Uint32 Buffer::GetHash(Void) const {

	StringC data = this->data();
	Uint32 len = this->length();

	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value

	unsigned int h = 0x7b1010 ^ len;

	// Mix 4 bytes at a time into the hash

	while (len >= 4)
	{
		unsigned int k = *(unsigned int *)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array

	switch (len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
		h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

LAME_END2
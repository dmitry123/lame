#ifndef __LAME_LANG__RANDOMIZER__
#define __LAME_LANG__RANDOMIZER__

#include "Foundation.h"

LAME_LANG_BEGIN

typedef class Randomizer {
	typedef Randomizer* RandomizerPtr;
public:
	static RandomizerPtr getDefault();
public:
	Randomizer(UInt32 length):
		buffer(new SInt8[length + 1]), length(length)
	{
	}
	virtual ~Randomizer() {
		delete[] this->buffer;
	}
public:
	StringPtrC randomize();
private:
	UInt32 length;
	StringPtr buffer;
} *RandomizerPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__RANDOMIZER__
#include "Randomizer.h"

#define LAME_LANG_RANDOMIZER_DEFAULT_BUFFER 32

LAME_LANG_BEGIN

RandomizerPtr randomizer = nullptr;

RandomizerPtr Randomizer::getDefault() {
	if (randomizer == nullptr) {
		return (randomizer = new Randomizer(LAME_LANG_RANDOMIZER_DEFAULT_BUFFER));
	}
	else {
		return randomizer;
	}
}

StringPtrC Randomizer::randomize() {

	UInt32 length = this->length / 2;

	memset(this->buffer, 0, this->length);

	while (length > 0) {

		UInt8 chunk = 0;

		for (UInt8 i = 0; i < sizeof(chunk) * 8; i++) {
			if (rand() % 2) {
				chunk |= 0x01 << i;
			}
		}

		sprintf(this->buffer + (this->length / 2 - length) / sizeof(chunk) * 2, "%.2x", chunk);

		length -= sizeof(chunk);
	}

	return this->buffer;
}

LAME_LANG_END
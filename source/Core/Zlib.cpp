#include "Utility.h"

#include "zlib/zlib.h"

LAME_BEGIN

Zlib::Zlib() {

	// reset fields
	this->buffer = 0;
	this->length = 0;
}

Zlib::~Zlib() {

	// release buffer
	if (this->buffer) {
		Free(this->buffer);
	}

	// reset fields
	this->buffer = 0;
	this->length = 0;
}

Bool Zlib::Compress(VoidP buffer, Uint32 length) {

	Sint32 value;

	// check buffer as allocated
	if (this->buffer) {
		Free(this->buffer);
	}

	// allocate buffer with current values
	this->buffer = Malloc(length);
	this->length = length;

	do {

		// try to compress data
		value = compress(
			// destination buffer
			(Bytef*) this->buffer,
			// destination buffer length
			(uLongf*)&this->length,
			// source buffer
			(const Bytef*)buffer,
			// source buffer length
			length);

		// check for z-lib errors
		if (value == Z_MEM_ERROR ||
			value == Z_DATA_ERROR
		) {
			// release buffer
			Free(this->buffer);
			// return failure
			PostErrorMessage("unable to compress, invalid data", 1);
		}

		// check for buffer erorr
		if (value == Z_BUF_ERROR) {
			// increase length
			this->length <<= 1;
			// set buffer to zero
			this->buffer = LAME_NULL;
			// realloc buffer
			this->buffer = Realloc(this->buffer, this->length);
		}

	} while (value != Z_OK);

	// realloc buffer for new compressed length
	this->buffer = Realloc(this->buffer, this->length);

	// return success
	return LAME_TRUE;
}

Bool Zlib::UnCompress(
	VoidP  buffer,
	Uint32 length)
{
	Sint32 value;

	// check buffer as allocated
	if (this->buffer) {
		Free(this->buffer);
	}

	// allocate buffer with current values
	this->buffer = Malloc(length);
	this->length = length;

	do {

		// try to compress data
		value = uncompress(
			// destination buffer
			(Bytef*) this->buffer,
			// destination buffer length
			(uLongf*)&this->length,
			// source buffer
			(const Bytef*)buffer,
			// source buffer length
			length);

		// check for z-lib errors
		if (value == Z_MEM_ERROR ||
			value == Z_DATA_ERROR
		) {
			// release buffer
			Free(this->buffer);
			// set buffer to zero
			this->buffer = LAME_NULL;
			// return failure
			PostErrorMessage("unable to uncompress, invalid data", 1);
		}

		// check for buffer erorr
		if (value == Z_BUF_ERROR) {
			// increase length
			this->length <<= 1;
			// realloc buffer
			this->buffer = Realloc(this->buffer, this->length);
		}

	} while (value != Z_OK);

	// realloc buffer for new compressed length
	this->buffer = Realloc(this->buffer, this->length);

	// return success
	return LAME_TRUE;
}

LAME_END
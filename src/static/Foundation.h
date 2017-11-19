#ifndef __LAME_STATIC_FOUNDATION__
#define __LAME_STATIC_FOUNDATION__

#include "../core/Foundation.h"

typedef struct LameStaticTextureContainer {
	UInt16 width;
	UInt16 height;
	UInt16 bitsPerPixel;
	UInt16 bytesPerPixel;
	UInt32 size;
	VoidP data;
	UInt32 jpegQuality;
} *LameStaticTextureContainerPtr;

typedef struct LameStaticAudioContainer {
	UInt32 length;
	UInt32 chunkSize;
	UInt32 formatType;
	UInt16 channels;
	UInt32 sampleRate;
	UInt32 avgBytesPerSec;
	UInt16 bytesPerSample;
	UInt16 bitsPerSample;
	VoidP data;
	UInt32 size;
} *LameStaticAudioContainerPtr;

#endif // ~__LAME_STATIC_FOUNDATION__
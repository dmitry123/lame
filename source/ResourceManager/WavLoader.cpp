#include "AudioLoader.h"

#include <string>

#ifndef _MSC_VER
#  define stricmp(_left, _right) strcasecmp((const char*)_left, (const char*)_right)
#else
#  define stricmp(_left, _right) _stricmp((const char*)_left, (const char*)_right)
#endif

LAME_BEGIN

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned int   dword;

#pragma pack(push, 1)
typedef struct {
    byte riffSig[4];
    dword size;
    byte waveSig[4];
    byte fmtSig[4];
    dword chunkSize;
    word formatType;
    word chanels;
    dword sampleRate;
    dword avgBytesPerSec;
    word bytesPerSample;
    word bitsPerSample;
    dword dataSize;
} WavHead, *WavHeadPtr;
#pragma pack(pop)

Bool WavLoader::Load(StringC fileName) {

    ResourceAudioInfoPtr audioInfo = this->GetInfo();
    
    File fileHandle;
//	FILE* file = NULL;

    fileHandle.Open(fileName, "rb");
//	file = fopen(fileName, "rb");

//	if (file == NULL) {
//		PostErrorMessage("Failed to open file", 1);
//	}

	audioInfo->BuildFileInfo(fileName);

	byte type[4];
	dword size;

    fileHandle.Read(type, 4);
//	fread(type, 1, 4, file);

    if (stricmp(type, "riff")) {
		PostErrorMessage("Not riff", 1);
    }

    fileHandle.Read(&size, sizeof(dword));
    fileHandle.Read(type, 4);
    
//	fread(&size, sizeof(dword), 1, file);
//	fread(type, sizeof(byte), 4, file);

    if (stricmp(type, "wave")) {
		PostErrorMessage("Not wave", 1);
    }

    fileHandle.Read(type, 4);
//	fread(type, sizeof(byte), 4, file);

    if (stricmp(type, "fmt ")) {
		PostErrorMessage("Not fmt", 1);
    }
    
    fileHandle.Read(&audioInfo->audioChunkSize, sizeof(dword));
    fileHandle.Read(&audioInfo->audioFormatType, sizeof(word));
    fileHandle.Read(&audioInfo->audioChannels, sizeof(word));
    fileHandle.Read(&audioInfo->audioSampleRate, sizeof(dword));
    fileHandle.Read(&audioInfo->audioAvgBytesPerSec, sizeof(dword));
    fileHandle.Read(&audioInfo->audioBytesPerSample, sizeof(word));
    fileHandle.Read(&audioInfo->audioBitsPerSample, sizeof(word));
    
//	fread(&audioInfo->audioChunkSize, sizeof(dword), 1, file);
//	fread(&audioInfo->audioFormatType, sizeof(word), 1, file);
//	fread(&audioInfo->audioChannels, sizeof(word), 1, file);
//	fread(&audioInfo->audioSampleRate, sizeof(dword), 1, file);
//	fread(&audioInfo->audioAvgBytesPerSec, sizeof(dword), 1, file);
//	fread(&audioInfo->audioBytesPerSample, sizeof(word), 1, file);
//	fread(&audioInfo->audioBitsPerSample, sizeof(word), 1, file);

    fileHandle.Read(type, 4);
    
//	fread(type, sizeof(byte), 4, file);
    
    if (stricmp(type, "data")) {
		PostErrorMessage("Missing data", 1);
    }

    fileHandle.Read(&audioInfo->audioDataSize, sizeof(dword));
//	fread(&audioInfo->audioDataSize, sizeof(dword), 1, file);
    
	audioInfo->audioData = new byte[audioInfo->audioDataSize];
    
    fileHandle.Read(audioInfo->audioData, audioInfo->audioDataSize);
//	fread(audioInfo->audioData, sizeof(byte), audioInfo->audioDataSize, file);

	return LAME_TRUE;
}

Bool WavLoader::Save(StringC fileName) {

    ResourceAudioInfoPtr audioInfo = this->GetInfo();
    WavHead head = { 0 };
    
    strcpy((String)head.riffSig, "RIFF");
    strcpy((String)head.waveSig, "WAVE");
    strcpy((String)head.fmtSig, "fmt ");
    
    head.chunkSize = audioInfo->audioChunkSize;
    head.formatType = audioInfo->audioFormatType;
    head.chanels = audioInfo->audioChannels;
    head.sampleRate = audioInfo->audioSampleRate;
    head.avgBytesPerSec = audioInfo->audioAvgBytesPerSec;
    head.bytesPerSample = audioInfo->audioBytesPerSample;
    head.bitsPerSample = audioInfo->audioBitsPerSample;
    head.dataSize = audioInfo->audioDataSize;
    
	PostErrorMessage("ResouceManager doesn't support WAV saving", 1);
    
	return LAME_FALSE;
}

LAME_END
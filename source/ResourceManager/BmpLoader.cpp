#include "TextureLoader.h"

LAME_BEGIN

#pragma pack(push, 2)
typedef struct {
	unsigned char magic1;
	unsigned char magic2;
	unsigned int size;
	unsigned short int reserved1, reserved2;
	unsigned int pixelOffset;
} BmpHeader;
#pragma pack(pop)

typedef struct {
	unsigned int size;
	int cols, rows;
	unsigned short int planes;
	unsigned short int bitsPerPixel;
	unsigned int compression;
	unsigned int cmpSize;
	int xScale, yScale;
	unsigned int numColors;
	unsigned int importantColors;
} BmpInfoHeader;

Bool BmpLoader::Load(StringC fileName) {

	File file;
	BmpHeader header;
	BmpInfoHeader infoHeader;
	Uint8P buffer;
	Uint32 size;
    
    ResourceTextureInfoPtr texInfo = this->GetInfo();

	texInfo->BuildFileInfo(fileName);

	file.Open(fileName, File::kRead | File::kBinary);
	file.Read(&header, sizeof(header));
	if (header.magic1 != 'B' || header.magic2 != 'M') {
		return LAME_FALSE;
	}
	file.Read(&infoHeader, sizeof(infoHeader));
	if (infoHeader.bitsPerPixel != 24) {
		return LAME_FALSE;
	}
	file.SetPosition(header.pixelOffset);
	size = (infoHeader.cols + 1) * (infoHeader.rows + 1) * infoHeader.bitsPerPixel / 8;
	buffer = (Uint8P)malloc(size);
	file.Read(buffer, size);
	file.Close();

	texInfo->imageWidth = infoHeader.cols;
	texInfo->imageHeight = infoHeader.rows;
	texInfo->imageBits = infoHeader.bitsPerPixel;
	texInfo->imageSize = size;
	texInfo->imageBytes = texInfo->imageBits / 8;
	texInfo->imageData = buffer;

	return LAME_TRUE;
}

Bool BmpLoader::Save(StringC fileName) {

	File file;
    
    ResourceTextureInfoPtr texInfo = this->GetInfo();

	BmpHeader header = { 'B', 'M',
		static_cast<unsigned int>(texInfo->imageSize + sizeof(BmpHeader) + sizeof(BmpInfoHeader)), 0, 0,
		sizeof(BmpHeader) + sizeof(BmpInfoHeader)
	};

	BmpInfoHeader infoHeader = {
		sizeof(BmpInfoHeader),
		texInfo->imageWidth,
		texInfo->imageHeight, 1,
		texInfo->imageBits, 0,
		static_cast<unsigned int>(texInfo->imageSize + sizeof(BmpHeader) + sizeof(BmpInfoHeader)), 0, 0, 0, 0
	};

	file.Open(fileName, File::kWrite | File::kBinary);
	file.Write(&header, sizeof(header));
	file.Write(&infoHeader, sizeof(infoHeader));
	file.Write(texInfo->imageData,
		(texInfo->imageWidth  + 1) *
		(texInfo->imageHeight + 1) * texInfo->imageBytes);
	file.Close();

	return LAME_TRUE;
}

LAME_END
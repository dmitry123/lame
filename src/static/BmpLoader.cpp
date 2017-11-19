#include "BmpLoader.h"

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

Logic LameStaticBmpLoader::load() {

	BmpHeader header;
	BmpInfoHeader infoHeader;
	UInt8P buffer;
	UInt32 size;
	FILE* file;

	file = fopen(this->getName(), "rb");
	fread(&header, sizeof(header), 1, file);
	if (header.magic1 != 'B' || header.magic2 != 'M') {
		return LAME_FALSE;
	}
	fread(&infoHeader, sizeof(infoHeader), 1, file);
	if (infoHeader.bitsPerPixel != 24) {
		return LAME_FALSE;
	}
	fseek(file, header.pixelOffset, SEEK_SET);
	size = (infoHeader.cols + 1) * (infoHeader.rows + 1) * infoHeader.bitsPerPixel / 8;
	buffer = (UInt8P) malloc(size);
	fread(buffer, size, 1, file);
	fclose(file);

	this->getData()->width = infoHeader.cols;
	this->getData()->height = infoHeader.rows;
	this->getData()->bitsPerPixel = infoHeader.bitsPerPixel;
	this->getData()->bytesPerPixel = infoHeader.bitsPerPixel / 8;
	this->getData()->size = size;

	UInt32 size24 = this->getData()->width * this->getData()->height * 3;
	UInt32 size32 = this->getData()->width * this->getData()->height * 4;

	this->getData()->data = buffer;

	return LAME_TRUE;

}

Logic LameStaticBmpLoader::save() {

	FILE* file;

	BmpHeader header = { 'B', 'M',
		static_cast<unsigned int>(this->getData()->size+ sizeof(BmpHeader) + sizeof(BmpInfoHeader)), 0, 0,
		sizeof(BmpHeader) + sizeof(BmpInfoHeader)
	};

	BmpInfoHeader infoHeader = { sizeof(BmpInfoHeader),
		this->getData()->width,
		this->getData()->height, 1,
		this->getData()->bitsPerPixel, 0,
		static_cast<unsigned int>(this->getData()->size + sizeof(BmpHeader) + sizeof(BmpInfoHeader)), 0, 0, 0, 0
	};

	file = fopen(this->getName(), "wb");
	fwrite(&header, sizeof(header), 1, file);
	fwrite(&infoHeader, sizeof(infoHeader), 1, file);
	UInt32 size = (this->getData()->width + 1) * (this->getData()->height + 1) * this->getData()->bytesPerPixel;
	fwrite(this->getData()->data, size, 1, file);
	fclose(file);

	return LAME_TRUE;
}
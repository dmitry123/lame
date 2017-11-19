#include "TgaLoader.h"

#pragma pack(push, 1)
typedef struct {
	char idlength;
	char colourmaptype;
	char datatypecode;
	short int colourmaporigin;
	short int colourmaplength;
	char colourmapdepth;
	short int x_origin;
	short int y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imagedescriptor;
} TgaHeader;
#pragma pack(pop)

typedef struct {
	unsigned char r, g, b, a;
} TgaPixel;

Void mergeBytes(TgaPixel* pixel, UInt8P p, UInt32 bytes) {

	if (bytes == 4) {
		pixel->r = p[2];
		pixel->g = p[1];
		pixel->b = p[0];
		pixel->a = p[3];
	}
	else if (bytes == 3) {
		pixel->r = p[2];
		pixel->g = p[1];
		pixel->b = p[0];
		pixel->a = 0;
	}
	else if (bytes == 2) {
		pixel->r = (p[1] & 0x7c) << 1;
		pixel->g = ((p[1] & 0x03) << 6) | ((p[0] & 0xe0) >> 2);
		pixel->b = (p[0] & 0x1f) << 3;
		pixel->a = (p[1] & 0x80);
	}
}

Logic LameStaticTgaLoader::load() {
	
	FILE* file;
	TgaHeader header;
	TgaPixel* pixels;
	UInt32 skipOver;
	UInt32 bytes2read;
	UInt32 i, j, n;
	UInt8 p[5];

	i = 0;
	n = 0;
	skipOver = 0;

	file = fopen(this->getName(), "rb");
	fread(&header, sizeof(header), 1, file);

	pixels = (TgaPixel*)malloc(header.width * header.height * sizeof(TgaPixel));

	if (header.datatypecode != 2 && header.datatypecode != 10) {
		throw std::exception("Can only handle image type 2 and 10", 1);
	}
	if (header.bitsperpixel != 16 && header.bitsperpixel != 24 && header.bitsperpixel != 32) {
		throw std::exception("Can only handle pixel depths of 16, 24, and 32", 1);
	}
	if (header.colourmaptype != 0 && header.colourmaptype != 1) {
		throw std::exception("Can only handle colour map types of 0 and 1", 1);
	}

	skipOver += header.idlength;
	skipOver += header.colourmaptype * header.colourmaplength;

	fseek(file, SEEK_CUR, skipOver);
	bytes2read = header.bitsperpixel / 8;

	while (n < (UInt32)header.width * header.height) {
		if (header.datatypecode == 2) { /* Uncompressed */
			if (fread(p, 1, bytes2read, file) != bytes2read) {
				throw std::exception("Unexpected end of file at pixel %d\n", i);
			}
			mergeBytes(&(pixels[n]), p, bytes2read);
			n++;
		}
		else if (header.datatypecode == 10) { /* Compressed */
			if (fread(p, 1, bytes2read + 1, file) != bytes2read + 1) {
				throw std::exception("Unexpected end of file at pixel %d\n", i);
			}
			j = p[0] & 0x7f;
			mergeBytes(&(pixels[n]), &(p[1]), bytes2read);
			n++;
			if (p[0] & 0x80) { /* RLE chunk */
				for (i = 0; i<j; i++) {
					mergeBytes(&(pixels[n]), &(p[1]), bytes2read);
					n++;
				}
			}
			else { /* Normal chunk */
				for (i = 0; i<j; i++) {
					if (fread(p, 1, bytes2read, file) != bytes2read) {
						throw std::exception("Unexpected end of file at pixel %d\n", i);
					}
					mergeBytes(&(pixels[n]), p, bytes2read);
					n++;
				}
			}
		}
	}
	fclose(file);

	this->getData()->bitsPerPixel = header.bitsperpixel;
	this->getData()->bytesPerPixel = header.bitsperpixel / 8;
	this->getData()->data = pixels;
	this->getData()->height = header.height;
	this->getData()->size = header.height * header.width * header.bitsperpixel / 8;
	this->getData()->width = header.width;

	return LAME_TRUE;
}

Logic LameStaticTgaLoader::save() {

	TgaHeader header = {
		0, 0, 2, 0, 0, 0, 0, 0,
		static_cast<short>(this->getData()->width),
		static_cast<short>(this->getData()->height),
		static_cast<char>(this->getData()->bitsPerPixel & 0xff), 8
	};

	FILE* file;

	file = fopen(this->getName(), "wb");
	fwrite(&header, sizeof(header), 1, file);
	fwrite(this->getData()->data, this->getData()->width * this->getData()->height * this->getData()->bytesPerPixel, 1, file);
	fclose(file);

	return LAME_TRUE;
}
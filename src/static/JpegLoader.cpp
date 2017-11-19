#include "JpegLoader.h"

#include "libjpeg\jpeglib.h"

#include <setjmp.h>

typedef struct MyErrorMessage {
	jpeg_error_mgr pub;
	jmp_buf setjmp_buffer;
} MyErrorMessagePtr;

Logic LameStaticJpegLoader::load() {

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	UInt8P raw_image;
	JSAMPROW row_pointer[1];
	UInt32 location = 0;
	UInt32 i = 0;
	FILE* infile;

	if (!(infile = fopen(this->getName(), "rb"))) {
		return FALSE;
	}

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);

	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);

	raw_image = (unsigned char*)malloc(cinfo.output_width * cinfo.output_height * cinfo.num_components);
	row_pointer[0] = (unsigned char *)malloc(cinfo.output_width * cinfo.num_components);

	while (cinfo.output_scanline < cinfo.image_height) {
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
		for (i = 0; i < cinfo.image_width * cinfo.num_components; i++) {
			raw_image[location++] = row_pointer[0][i];
		}
	}

	this->getData()->width = cinfo.image_width;
	this->getData()->height = cinfo.image_height;
	this->getData()->bytesPerPixel = cinfo.out_color_components;
	this->getData()->bitsPerPixel = cinfo.output_components * 8;
	this->getData()->size = cinfo.image_width * cinfo.image_height * cinfo.output_components;
	this->getData()->data = raw_image;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
	fclose(infile);

	return LAME_TRUE;
}

Logic LameStaticJpegLoader::save() {
	
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	FILE *outfile = fopen(this->getName(), "wb");
	UInt8P raw_image;

	if (!outfile) {
		return 0;
	}

	if (this->getData()->jpegQuality > 100) {
		this->getData()->jpegQuality = 100;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = this->getData()->width;
	cinfo.image_height = this->getData()->height;
	cinfo.input_components = this->getData()->bytesPerPixel;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_quality(&cinfo, this->getData()->jpegQuality, FALSE);
	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	raw_image = (UInt8P)this->getData()->data;

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &raw_image[cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);

	return LAME_TRUE;
}
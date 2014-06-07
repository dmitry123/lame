#include "libjpeg/jpeglib.h"

#include "TextureLoader.h"

#include <setjmp.h>

LAME_BEGIN

struct my_error_mgr {
	jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

Bool JpgLoader::Load(StringC fileName) {

    ResourceTextureInfoPtr texInfo = this->GetInfo();
    
	texInfo->BuildFileInfo(fileName);

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	Uint8P raw_image;
	JSAMPROW row_pointer[1];

	Uint32 location = 0;
	Uint32 i = 0;

	FILE *infile = fopen(fileName, "rb");

	if (!infile) {
		return 0;
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

		for (i = 0; i < cinfo.image_width * cinfo.num_components; i++)
			raw_image[location++] = row_pointer[0][i];
	}

	texInfo->imageWidth = cinfo.image_width;
	texInfo->imageHeight = cinfo.image_height;
	texInfo->imageBytes = cinfo.out_color_components;
	texInfo->imageBits = cinfo.output_components * 8;
	texInfo->imageSize = cinfo.image_width * cinfo.image_height * cinfo.output_components;
	texInfo->imageData = raw_image;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(row_pointer[0]);
	fclose(infile);

	return LAME_TRUE;
}

Bool JpgLoader::Save(StringC fileName) {

    ResourceTextureInfoPtr texInfo = this->GetInfo();
    
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];
	FILE *outfile = fopen(fileName, "wb");
	Uint8P raw_image;

	if (!outfile) {
		return 0;
	}

	if (texInfo->imageJpegQuality > 100) {
		texInfo->imageJpegQuality = 100;
	}

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = texInfo->imageWidth;
	cinfo.image_height = texInfo->imageHeight;
	cinfo.input_components = texInfo->imageBytes;
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_quality(&cinfo, texInfo->imageJpegQuality, FALSE);
	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	raw_image = (Uint8P)texInfo->imageData;

	while (cinfo.next_scanline < cinfo.image_height) {

		row_pointer[0] = &raw_image[cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	fclose(outfile);

	return LAME_TRUE;
}

LAME_END
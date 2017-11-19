#include "PngLoader.h"

#define PNG_SIMPLIFIED_WRITE_SUPPORTED
#define PNG_SIMPLIFIED_READ_SUPPORTED
#define PNG_STDIO_SUPPORTED

#include "libpng/png.h"

Logic LameStaticPngLoader::load() {

	png_image image = { 0 };
	png_bytep buffer = 0;

	image.version = PNG_IMAGE_VERSION;

	if (!png_image_begin_read_from_file(&image, this->getName())) {
		return FALSE;
	}

	image.format = PNG_FORMAT_RGB;
	buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
	png_image_finish_read(&image, NULL, buffer, 0, NULL);

	this->getData()->width = image.width;
	this->getData()->height = image.height;
	this->getData()->size = PNG_IMAGE_SIZE(image);
	this->getData()->bytesPerPixel = this->getData()->size / image.width / image.height;
	this->getData()->bitsPerPixel = this->getData()->bytesPerPixel / 8;
	this->getData()->data = buffer;

	return LAME_TRUE;
}

Logic LameStaticPngLoader::save() {

	png_image image = { 0 };
	png_bytep buffer = 0;

	image.version = PNG_IMAGE_VERSION;
	image.width = this->getData()->width;
	image.height = this->getData()->height;
	buffer = (png_bytep)this->getData()->data;
	image.colormap_entries = image.width;

	if (this->getData()->bitsPerPixel != 32) {
		image.format = PNG_FORMAT_RGB;
	}
	else {
		image.format = PNG_FORMAT_RGBA;
	}

	if (!png_image_write_to_file(&image, this->getName(), 0, buffer, 0, NULL)) {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}
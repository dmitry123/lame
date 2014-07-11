#include "TextureLoader.h"

#define PNG_SIMPLIFIED_WRITE_SUPPORTED
#define PNG_SIMPLIFIED_READ_SUPPORTED
#define PNG_STDIO_SUPPORTED

#include "libpng/png.h"

LAME_BEGIN2(ResourceManager)

Bool PngLoader::Load(StringC fileName) {

    ResourceTextureInfoPtr texInfo = this->GetInfo();
    
	png_image image = { 0 };
	png_bytep buffer = 0;

	texInfo->BuildFileInfo(fileName);

	image.version = PNG_IMAGE_VERSION;

	if (!png_image_begin_read_from_file(&image, fileName)) {
		throw ResourceException("Unable to read png file (%s)", image.message);
	}

	image.format = PNG_FORMAT_RGB;
	buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));
	png_image_finish_read(&image, NULL, buffer, 0, NULL);

	texInfo->imageWidth = image.width;
	texInfo->imageHeight = image.height;
    texInfo->imageSize = PNG_IMAGE_SIZE(image);
    texInfo->imageBytes = PNG_IMAGE_SIZE(image) / image.width / image.height;
    texInfo->imageBits = texInfo->imageBytes * 8;
    texInfo->imageData = buffer;

	return LAME_TRUE;
}

Bool PngLoader::Save(StringC fileName) {

    ResourceTextureInfoPtr texInfo = this->GetInfo();
    
	png_image image = { 0 };
	png_bytep buffer = 0;

	image.version = PNG_IMAGE_VERSION;
	image.width = texInfo->imageWidth;
	image.height = texInfo->imageHeight;
	buffer = (png_bytep)texInfo->imageData;
	image.colormap_entries = image.width;

	if (texInfo->imageBits != 32) {
		image.format = PNG_FORMAT_RGB;
	} else {
		image.format = PNG_FORMAT_RGBA;
	}

	if (!png_image_write_to_file(&image, fileName, 0, buffer, 0, NULL)) {
		return LAME_FALSE;
	}

	return LAME_TRUE;
}

LAME_END2

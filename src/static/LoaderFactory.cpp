#include "LoaderFactory.h"

#include "JpegLoader.h"
#include "PngLoader.h"
#include "BmpLoader.h"
#include "TgaLoader.h"

static LameStaticLoaderFactoryPtr factory = NULL;

LameStaticLoaderFactory::LameStaticLoaderFactoryP LameStaticLoaderFactory::getFactory() {
	if (factory == NULL) {
		return (factory = new LameStaticLoaderFactory());
	}
	else {
		return factory;
	}
}

LameStaticAbstractLoaderPtr LameStaticLoaderFactory::createByName(StringPtrC filename) {

	StringPtrC ptr = filename,
		ext = filename;

	LameStaticAbstractLoaderPtr loader = NULL;

	while (ptr != NULL && (ptr = strchr(ptr + 1, '.'))) {
		ext = ptr + 1;
	}

	if (!strcmp(ext, "jpg")) {
		loader = new LameStaticJpegLoader(filename);
	}
	else if (!stricmp(ext, "png")) {
		loader = new LameStaticPngLoader(filename);
	}
	else if (!stricmp(ext, "tga")) {
		loader = new LameStaticTgaLoader(filename);
	}
	else if (!stricmp(ext, "bmp")) {
		loader = new LameStaticBmpLoader(filename);
	}

	if (loader != NULL) {
		this->loaders.push_back(loader);
	}

	return loader;
}
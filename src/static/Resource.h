#ifndef __LAME_STATIC_RESOURCE__
#define __LAME_STATIC_RESOURCE__

#include "Foundation.h"
#include "AbstractLoader.h"
#include "LoaderFactory.h"

template <class DataType>
class LameStaticResource {
	typedef LameStaticAbstractLoaderPtr Loader;
public:
	typedef enum {
		TYPE_UNKNOWN,
		TYPE_BINARY,
		TYPE_TEXT,
		TYPE_TEXTURE,
		TYPE_MAP,
		TYPE_AUDIO,
		TYPE_VIDEO
	} Type;
public:
	LameStaticResource(StringPtrC filename, Type type) :
		type(type)
	{
		this->loader = LameStaticLoaderFactory::getFactory()
			->createByName(filename);
	}
public:
	inline Logic save() {
		this->loader->save();
	}
	inline Logic load() {
		this->loader->load();
	}
public:
	inline const DataType* getData() const {
		return this->data;
	}
	inline Type getType() const {
		return this->type;
	}
	inline Loader getLoader() {
		return this->loader;
	}
private:
	Loader loader;
	Type type;
	DataType data;
};

#endif // ~__LAME_STATIC_RESOURCE__
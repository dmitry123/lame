#ifndef __LAME_CORE_DYNAMIC__
#define __LAME_CORE_DYNAMIC__

#include "Types.h"
#include "File.h"
#include "Exception.h"

LAME_BEGIN2(Core)

class LAME_API Library {
public:
	typedef VoidP MemoryModule;
	typedef VoidP MemoryResource;
	typedef VoidP CustomModule;
	typedef VoidP ImageNtHeader;
public:
	Library() :
		blockData(NULL)
	{
	}
	Library(StringC fileName) : Library() {
		this->Load(fileName);
	}
public:
	~Library() {
		for (LibraryPtr m : this->moduleList) {
			delete m;
		}
		delete (Uint8P)this->blockData;
	}
public:
	Void Load(StringC fileName);
	VoidP GetProcAddress(StringC procName);
public:
	inline Void AddModule(LibraryPtr module) {
		this->moduleList.insert(module);
	}
public:
	inline Bool IsInitialized() {
		return this->isInitialized;
	}
	inline Bool IsAmd64() {
		return this->isAmd64;
	}
public:
	inline ImageNtHeader GetHeader()      { return this->ptrHeaders;   }
	inline VoidP         GetCodeSegment() { return this->codeSegment;  }
	inline Buffer        GetName()        { return this->fileName;     }
	inline VoidP         GetNative()      { return this->nativeHandle; }
	inline VoidP         GetData()        { return this->blockData;    }
private:
	VoidP codeSegment;
	Uint8P ptrHeaders;
	Bool isInitialized;
	Buffer fileName;
	VoidP nativeHandle;
	VoidP blockData;
	Bool isAmd64;
private:
	Set<LibraryPtr> moduleList;
};

LAME_END2

#endif // ~__LAME_CORE_DYNAMIC__
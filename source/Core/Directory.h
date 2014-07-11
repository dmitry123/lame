#ifndef __LAME_CORE__DIRECTORY__
#define __LAME_CORE__DIRECTORY__

#include "Types.h"
#include "SharedPtr.h"
#include "List.h"
#include "Exception.h"
#include "Buffer.h"

LAME_BEGIN2(Core)

class LAME_API Directory {
public:
	typedef enum  {
		kDefault = 0x0000,
		kFiles = 0x0001,
		kDirectories = 0x0002,
		kDots = 0x0004,
		kDeep = 0x0008,
		kReadble = 0x0010,
		kWritable = 0x0020,
		kExecutable = 0x0040,
		kHidden = 0x0080,
		kSystem = 0x0100
	} FilterFlags;
public:
	typedef class Fpc {
	public:
		inline Fpc() {
			ZeroMemory(this->path_, sizeof(this->path_));
		}
		inline Fpc(StringC path) {
			strcpy(this->path_, path);
		}
		inline Void operator = (StringC path) {
			strcpy(this->path_, path);
		}
	public:
		inline operator String () { return this->path_; }
		inline operator StringC () { return this->path_; }
	private:
		FilePathBuffer path_;
	} Fpc;
public:
	typedef List <Fpc> Stack;
public:
	Void Open(StringC path, Uint32 flags, StringC extfilter = 0);
	StringC Next(Void);
	Void Close(Void);
	List <Buffer> GetList(Void);
public:
	inline StringC GetLast(Void) const {
		return this->last_;
	}
	inline Void SetExtensionFilter(StringC filter) {
		this->extfilter_ = filter;
		this->extlist_ = Buffer(filter).Split('&');
	}
	inline Void RemoveExtensionFilter(Void) {
		this->extfilter_ = 0;
		this->extlist_.clear();
	}
public:
	 Directory();
	~Directory();
public:
	static List <Buffer> GetContentAtPath(StringC path);
	static List <Buffer> GetDirectoriesAtPath(StringC path);
	static List <Buffer> GetFilesAtPath(StringC path, StringC extensions = LAME_NULL);
	static List <Buffer> GetContentAtPathWithDeep(StringC path);
	static List <Buffer> GetDirectoriesAtPathWithDeep(StringC path);
	static List <Buffer> GetFilesAtPathWithDeep(StringC path, StringC extensions = LAME_NULL);
	static Buffer FindFileAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
	static Buffer FindFileAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);
	static List <Buffer> FindFilesAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
	static List <Buffer> FindFilesAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);
	static List <Buffer> FindContainingFilesAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
	static List <Buffer> FindContainingFilesAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);
private:
	VoidP handle_;
	FilePathBuffer path_;
	Uint32 flags_;
	FilePathBuffer last_;
	Stack stack_;
	StringC extfilter_;
	List <Buffer> extlist_;
};

LAME_END2

#endif // ~__LAME_CORE__DIRECTORY__
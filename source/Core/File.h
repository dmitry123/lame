#ifndef __LAME_CORE__FILE__
#define __LAME_CORE__FILE__

#include "Types.h"
#include "Timer.h"

LAME_BEGIN2(Core)

class LAME_API File {
public:
	typedef enum {
		kPermissionDefault = 0x0000,
		kPermissionRead = 0x0001,
		kPermissionWrite = 0x0002,
		kPermissionExecute = 0x0004
	} Permission;
	typedef enum {
		kTypeDefault = 0x00000,
		kTypeFile = 0x00001,
		kTypeDirectory = 0x00002
	} Type;
	typedef enum {
		kDefault = 0x0000,
		kRead = 0x0001,
		kWrite = 0x0002,
		kAppend = 0x0004,
		kText = 0x0008,
		kBinary = 0x0010,
		kClear = 0x0020,
		kLoad = 0x0040
	} Flags;
public:
	Void Open(StringC filename, Uint32 flags);
	Void Open(StringC filename, StringC mode);
	Uint32 Write(VoidP buffer, Uint32 length);
	Uint32 Read(VoidP buffer, Uint32 length);
	Void Close(Void);
	Uint32 GetPosition(Void);
	Uint64 GetPosition64(Void);
	Uint32 SetPosition(Uint32 position);
	Uint64 SetPosition64(Uint64 position);
	Bool Rewind(Void);
	Time GetModificationDate(Void);
	Time GetCreationDate(Void);
	Time GetAccessDate(Void);
public:
	inline StringC GetFileName() const { return this->filename_.data(); }
	inline Uint32 GetSize() const { return (Uint32)this->size_; }
	inline Uint64 GetSize64() const { return this->size_; }
	inline VoidP GetHandle() const { return this->handle_; }
	inline Uint32 GetFlags() const { return this->flags_; }
	inline Permission GetPermission() const { return this->permission_; }
	inline Type GetType() const { return this->type_; }
public:
	inline Bool CheckPermission(Permission permission) const {
		return this->permission_ & permission;
	}
	inline Bool CheckType(Type type) const {
		return this->type_ == type;
	}
public:
	 File();
	~File();
public:
	static Uint32 GetFileSize(StringC filename);
	static Uint32 GetFileSize64(StringC filename);
	static StringC GetCurrentDirectory(Void);
	static Bool ChangeDirectory(StringC name);
	static Buffer NormalizePath(StringC buffer);
	static Void NormalizePath(Sint8P buffer);
	static StringC GetFileExtension(StringC filename);
	static Bool CheckFileExtension(StringC filename, StringC exts);
	static Bool CheckFileExtension(StringC filename, const List <Buffer>& extList);
	static StringC GetFileName(StringC filename);
	static Buffer GetFileNameWithoutExtension(StringC filename);
	static StringC GetPathWithoutBegin(StringC filename);
	static Buffer GetPathWithoutEnd(StringC filename);
public:
	static Bool IsDirectory(StringC name);
	static Bool IsFile(StringC name);
	static Bool IsContent(StringC name);
	static Bool CreateDirectory(StringC name);
	static Bool CreateFile(StringC name);
	static Bool RemoveDirectory(StringC name);
	static Bool RemoveFile(StringC name);
	static Bool RemoveContent(StringC name);
	static Bool RenameDirectory(StringC name, StringC result);
	static Bool RenameFile(StringC name, StringC result);
	static Bool RenameContent(StringC name, StringC result);
	static Bool MoveDirectory(StringC name, StringC result);
	static Bool MoveFile(StringC name, StringC result);
	static Bool MoveContent(StringC name, StringC result);
	static Bool CopyDirectory(StringC name, StringC result);
	static Bool CopyFile(StringC name, StringC result);
	static Bool CopyContent(StringC name, StringC result);
	static Bool CreateDirectoryTree(StringC name);
	static Bool RemoveDirectoryTree(StringC name);
	static Bool RenameDirectoryTree(StringC name, StringC result);
	static Bool MoveDirectoryTree(StringC name, StringC result);
	static Bool CopyDirectoryTree(StringC name, StringC result);
private:
	Buffer filename_;
	Uint64 size_;
	VoidP handle_;
	Uint32 flags_;
	Permission permission_;
	Type type_;
};

LAME_END2

#endif // ~__LAME_CORE__FILE__
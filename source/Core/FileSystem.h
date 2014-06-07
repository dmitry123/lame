#ifndef __LAME_CORE__FILE_SYSTEM__
#define __LAME_CORE__FILE_SYSTEM__

#include "Define.h"
#include "Types.h"
#include "Utility.h"
#include "Timer.h"

LAME_BEGIN

typedef char FileSignature[LAME_SIGNATURE_MAX];
typedef char FileName[LAME_FILENAME_MAX];
typedef char FilePath[LAME_FILEPATH_MAX];
typedef char FileExtension[LAME_FILEEXTENSION_MAX];

Uint32 LAME_API GetFileSize(StringC filename);
Uint32 LAME_API GetFileSize64(StringC filename);
StringC LAME_API GetCurrentDirectory(Void);
Bool LAME_API ChangeDirectory(StringC name);
Buffer LAME_API NormalizePath(StringC buffer);
Void LAME_API NormalizePath(Sint8P buffer);
StringC LAME_API GetFileExtension(StringC filename);
Bool LAME_API CheckFileExtension(StringC filename, StringC exts);
Bool LAME_API CheckFileExtension(StringC filename, const List <Buffer>& extList);
StringC LAME_API GetFileName(StringC filename);
Buffer LAME_API GetFileNameWithoutExtension(StringC filename);
StringC LAME_API GetPathWithoutBegin(StringC filename);
Buffer LAME_API GetPathWithoutEnd(StringC filename);

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
private:
	Buffer filename_;
	Uint64 size_;
	VoidP handle_;
	Uint32 flags_;
	Permission permission_;
	Type type_;
};

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
	typedef class FPC {
	public:
		inline FPC() { ZeroMemory(this->path_, sizeof(this->path_)); }
		inline FPC(StringC path) { strcpy(this->path_, path); }
		inline Void operator = (StringC path) { strcpy(this->path_, path); }
		inline operator String () { return this->path_; }
		inline operator StringC () { return this->path_; }
	private:
		FilePathBuffer path_;
	} FPC;
public:
	typedef List <FPC> Stack;
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
private:
	VoidP handle_;
	FilePathBuffer path_;
	Uint32 flags_;
	FilePathBuffer last_;
	Stack stack_;
	StringC extfilter_;
	List <Buffer> extlist_;
};

class FilePathInfo {
public:
	FilePathInfo() {
	}
public:
	FilePathInfo(StringC buffer) {
		this->Parse(buffer);
	}
public:
	inline Void Parse(StringC buffer) {
		this->name = GetFileNameWithoutExtension(buffer);
		this->extension = GetFileExtension(buffer);
		this->path = GetPathWithoutEnd(buffer);
		this->buffer = NormalizePath(buffer);
	}
	inline Void Create() {
		this->buffer.clear();
		this->buffer += this->path;
		this->buffer += LAME_SLASH;
		this->buffer += this->name;
		this->buffer += '.';
		this->buffer += this->extension;
	}
public:
	inline operator StringC() const {
		return this->buffer.data();
	}
public:
	Buffer buffer;
	Buffer path;
	Buffer name;
	Buffer extension;
};

Bool LAME_API IsDirectory(StringC name);
Bool LAME_API IsFile(StringC name);
Bool LAME_API IsContent(StringC name);
Bool LAME_API CreateDirectory(StringC name);
Bool LAME_API CreateFile(StringC name);
Bool LAME_API RemoveDirectory(StringC name);
Bool LAME_API RemoveFile(StringC name);
Bool LAME_API RemoveContent(StringC name);
Bool LAME_API RenameDirectory(StringC name, StringC result);
Bool LAME_API RenameFile(StringC name, StringC result);
Bool LAME_API RenameContent(StringC name, StringC result);
Bool LAME_API MoveDirectory(StringC name, StringC result);
Bool LAME_API MoveFile(StringC name, StringC result);
Bool LAME_API MoveContent(StringC name, StringC result);
Bool LAME_API CopyDirectory(StringC name, StringC result);
Bool LAME_API CopyFile(StringC name, StringC result);
Bool LAME_API CopyContent(StringC name, StringC result);
Bool LAME_API CreateDirectoryTree(StringC name);
Bool LAME_API RemoveDirectoryTree(StringC name);
Bool LAME_API RenameDirectoryTree(StringC name, StringC result);
Bool LAME_API MoveDirectoryTree(StringC name, StringC result);
Bool LAME_API CopyDirectoryTree(StringC name, StringC result);

List <Buffer> LAME_API GetContentAtPath(StringC path);
List <Buffer> LAME_API GetDirectoriesAtPath(StringC path);
List <Buffer> LAME_API GetFilesAtPath(StringC path, StringC extensions = LAME_NULL);
List <Buffer> LAME_API GetContentAtPathWithDeep(StringC path);
List <Buffer> LAME_API GetDirectoriesAtPathWithDeep(StringC path);
List <Buffer> LAME_API GetFilesAtPathWithDeep(StringC path, StringC extensions = LAME_NULL);
Buffer LAME_API FindFileAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
Buffer LAME_API FindFileAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);
List <Buffer> LAME_API FindFilesAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
List <Buffer> LAME_API FindFilesAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);
List <Buffer> LAME_API FindContainingFilesAtPath(StringC path, StringC name, StringC extensions = LAME_NULL);
List <Buffer> LAME_API FindContainingFilesAtPathWithDeep(StringC path, StringC name, StringC extensions = LAME_NULL);

LAME_END

#endif // ~__LAME_CORE__FILE_SYSTEM__
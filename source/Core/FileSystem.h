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
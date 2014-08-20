#include "Directory.h"
#include "File.h"

#ifdef LAME_WINDOWS
#  include <Windows.h>
#else
#  include <dirent.h>
#  include <unistd.h>
#endif

#include <sys/stat.h>

#ifdef LAME_WINDOWS
#  define __FindFirstFile(_dirname) FindFirstFileA(_dirname, &file)
#  define __FindNextFile(_handle) FindNextFileA(_handle, &file)
#  define __FindClose(_handle) FindClose(_handle)
#else
#  define __FindFirstFile(_dirname) opendir(_dirname)
#  define __FindNextFile(_handle) readdir((DIR*)_handle)
#  define __FindClose(_handle) closedir((DIR*)_handle) == 0
#endif

#ifdef LAME_WINDOWS
#  undef GetMessage
#  undef GetCurrentDirectory
#endif

#ifdef LAME_WINDOWS
#  define FLAG_HIDDEN FILE_ATTRIBUTE_HIDDEN
#  define FLAG_RWX (S_IREAD | S_IWRITE | S_IEXEC)
#  define FLAG_READABLE S_IREAD 
#  define FLAG_WRITABLE S_IWRITE
#  define FLAG_EXECUTABE S_IEXEC
#  define FLAG_SYSTEM FILE_ATTRIBUTE_SYSTEM
#else
#  define FLAG_HIDDEN UF_HIDDEN
#  define FLAG_RWX S_IRWXU
#  define FLAG_READABLE S_IRUSR
#  define FLAG_WRITABLE S_IWUSR
#  define FLAG_EXECUTABE S_IXUSR
#  define FLAG_SYSTEM UF_IMMUTABLE
#endif

typedef struct stat Stat;

#ifdef LAME_WINDOWS
typedef BOOL Dirent;
#else
typedef struct dirent* Dirent;
#endif

#ifdef LAME_WINDOWS
#  define strcasecmp _stricmp
#endif

#ifndef LAME_WINDOWS
#  define INVALID_HANDLE_VALUE (VoidP)-1
#endif

LAME_BEGIN2(Core)

Directory::Directory() {

	this->handle_ = 0;
	this->flags_ = 0;
	this->extfilter_ = 0;

	memset(this->path_, 0, sizeof(this->path_));
	memset(this->last_, 0, sizeof(this->last_));
}

Directory::~Directory() {

	if (this->handle_) {
		this->Close();
	}
}

Void Directory::Open(StringC path, Uint32 flags, StringC extfilter) {

	FilePathBuffer searchPath;

#ifdef LAME_WINDOWS
	WIN32_FIND_DATAA file;
#endif

	if (this->handle_) {
		this->Close();
	}
    
    if (!flags) {
        flags = kDirectories | kFiles;
    }
    
    if (!path || path[0] == '\0') {
        path = File::GetCurrentDirectory();
    }

	strcpy(searchPath, path);

#ifdef LAME_WINDOWS
	strcat(searchPath, "\\*.*");
#endif

	this->handle_ = __FindFirstFile(searchPath);

	if (!this->handle_ || this->handle_ == INVALID_HANDLE_VALUE) {
		throw DirectoryException("Unable to open directory (%s)", path);
	}

    if (!(this->flags_ & kDots)) {
        __FindNextFile(this->handle_);
    }
    
    strcpy(this->path_, path);

	this->flags_ = flags;

	if (extfilter) {
		this->SetExtensionFilter(extfilter);
	}
}

StringC Directory::Next(Void) {

#ifdef LAME_WINDOWS
	WIN32_FIND_DATAA file;
#endif

	Dirent entry;
	Stat st;
	Uint32 flags;

	if (!this->handle_) {
		return LAME_NULL;
	}

__RepeatFileSearch:;

	memset(this->last_, 0, sizeof(this->last_));

	if (!(entry = __FindNextFile(this->handle_))) {

		Uint32 flags = this->flags_;

		if (!this->stack_.size()) {
			return LAME_NULL;
		}

		this->Close();

	__TryToOpenAnotherDirectory:

		try {
			this->Open(this->stack_.back(), flags);
		}
		catch(Exception error) {

			this->stack_.pop_back();

			if (this->stack_.size()) {
				goto __TryToOpenAnotherDirectory;
			}
			else {
				return LAME_NULL;
			}
		}

		if (this->stack_.size() == 0) {
			return LAME_NULL;
		}

		this->stack_.pop_back();

		goto __RepeatFileSearch;
	}

	strcpy(this->last_, this->path_);
    
    if (this->last_[strlen(this->last_) - 1] != LAME_SLASH) {
        this->last_[strlen(this->last_)] = LAME_SLASH;
    }

    StringC filename;
    
#ifdef LAME_WINDOWS
    filename = file.cFileName;
	flags = file.dwFileAttributes;
#else
    filename = entry->d_name;
#endif

    strcat(this->last_, filename);
    
    if (!(this->flags_ & kSystem) && *filename == '.') {
        goto __RepeatFileSearch;
    }
    
    File::NormalizePath(this->last_);

	if (stat(this->last_, &st) == 0) {
        
#ifndef LAME_WINDOWS
		flags = st.st_flags;
#endif

        // check content for hidden
		if (flags & FLAG_HIDDEN && !(this->flags_ & kHidden)) {
            goto __RepeatFileSearch;
        }
        
        // check content for system
		if (flags & FLAG_SYSTEM && !(this->flags_ & kSystem)) {
            goto __RepeatFileSearch;
        }
        
        // check content for dir
		if (st.st_mode & S_IFDIR) {
            
            // check content for deep
			if (this->flags_ & kDeep) {
				this->stack_.push_back(this->last_);
			}
            
            // check dir for dirs
			if (!(this->flags_ & kDirectories)) {
				goto __RepeatFileSearch;
			}
		}
		else {

			// check file's extension
			if (this->extfilter_ && !File::CheckFileExtension(filename, this->extlist_)) {
				goto __RepeatFileSearch;
			}
            
            // check dir for files
			if (!(this->flags_ & kFiles)) {
				goto __RepeatFileSearch;
			}
            else {
                
                // check file for RWX
				if (st.st_mode & FLAG_RWX && this->flags_ & kExecutable &&
											 this->flags_ & kWritable &&
											 this->flags_ & kReadble) {
                    goto __RepeatFileSearch;
                }
                
                // check for R
				if (st.st_mode & FLAG_READABLE && this->flags_ & kReadble) {
                    goto __RepeatFileSearch;
                }
                
                // check for W
                if (st.st_mode & FLAG_WRITABLE && this->flags_ & kWritable) {
                    goto __RepeatFileSearch;;
                }
                
                // check for E
                if (st.st_mode & FLAG_EXECUTABE && this->flags_ & kExecutable) {
                    goto __RepeatFileSearch;
                }
            }
		}
	}

	return this->last_;
}

Void Directory::Close(Void) {

	if (this->handle_) {
		__FindClose(this->handle_);
	}

	this->handle_ = 0;
	this->flags_ = 0;

	memset(this->path_, 0, sizeof(this->path_));
	memset(this->last_, 0, sizeof(this->last_));
}

List <Buffer> Directory::GetList(Void) {

	List <Buffer> list;

	while (this->Next()) {
		list.push_back(this->GetLast());
	}

	return list;
}

static List <Buffer> __GetContentAtPath(StringC path, StringC ext, Uint32 flags) {

	Directory dir;
	List <Buffer> list;
	List <Buffer> extList;

	if (ext) {
		extList = Buffer(ext).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (ext) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(File::GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		list.push_back(dir.GetLast());
	}
	dir.Close();

	return list;
}

static Buffer __FindFileAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(File::GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strcasecmp(File::GetFileNameWithoutExtension(dir.GetLast() + 1).data(), name) == 0) {
			return Buffer(dir.GetLast());
		}
	}
	dir.Close();

	return Buffer();
}

static List <Buffer> __FindFilesAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> result;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(File::GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strcasecmp(File::GetFileNameWithoutExtension(dir.GetLast() + 1).data(), name) == 0) {
			result.push_back(dir.GetLast());
		}
	}
	dir.Close();

	return result;
}

static List <Buffer> __FindContainingFilesAtPath(StringC path, StringC name, StringC extensions, Uint32 flags) {

	Directory dir;
	List <Buffer> result;
	List <Buffer> extList;

	if (extensions) {
		extList = Buffer(extensions).Split('&');
	}

	dir.Open(path, flags);
	while (dir.Next()) {
		if (extensions) {
			Bool state = LAME_FALSE;
			for (const Buffer& ext_ : extList) {
				if (strcasecmp(File::GetFileExtension(dir.GetLast()), ext_.data()) == 0) {
					state = LAME_TRUE;
				}
			}
			if (!state) {
				continue;
			}
		}
		if (strstr(dir.GetLast(), name)) {
			result.push_back(dir.GetLast());
		}
	}
	dir.Close();

	return result;
}

List <Buffer> Directory::GetContent(StringC path, Bool withDepth) {
    
    if (withDepth) {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kFiles |
            Directory::kDirectories |
            Directory::kDeep);
    } else {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kFiles |
            Directory::kDirectories);
    }
}

List <Buffer> Directory::GetDirectories(StringC path, Bool withDepth) {
    
    if (withDepth) {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kDirectories |
            Directory::kDeep);
    } else {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kDirectories);
    }
}

List <Buffer> Directory::GetFiles(StringC path, Bool withDepth, StringC extension) {
    
    if (withDepth) {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kFiles |
            Directory::kDeep);
    } else {
        return __GetContentAtPath(path, LAME_NULL,
            Directory::kFiles);
    }
}

Buffer Directory::FindFile(StringC path, StringC name, Bool withDepth, StringC extensions) {
    
    if (withDepth) {
        return __FindFileAtPath(path, name, extensions,
            Directory::kFiles |
            Directory::kDeep);
    } else {
        return __FindFileAtPath(path, name, extensions,
            Directory::kFiles);
    }
}

List <Buffer> Directory::FindFiles(StringC path, StringC name, Bool withDepth, StringC extensions) {
    
    if (withDepth) {
        return __FindFilesAtPath(path, name, extensions,
            Directory::kFiles |
            Directory::kDeep);
    } else {
        return __FindFilesAtPath(path, name, extensions,
            Directory::kFiles);
    }
}

List <Buffer> Directory::FindContainingFiles(StringC path, StringC name, Bool withDepth, StringC extensions) {
    
    if (withDepth) {
        return __FindContainingFilesAtPath(path, name, extensions,
            Directory::kFiles |
            Directory::kDeep);
    } else {
        return __FindContainingFilesAtPath(path, name, extensions,
            Directory::kFiles);
    }
}

LAME_END2

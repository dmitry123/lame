#ifndef __LAME_SCRIPT_PACKAGE__
#define __LAME_SCRIPT_PACKAGE__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API Package {
public:
	Package(BufferRefC javaCorePath = "") :
		javaCorePath(javaCorePath)
	{
	}
public:
	~Package();
public:
	Void Register(NodePtr node);
	PackagePtr Import(NodePtr node);
public:
	inline Set<PackagePtr>& GetPackages() {
		return this->packageSet;
	}
	inline Bool IsRegistered() {
		return !this->packagePath.empty();
	}
public:
	inline Void SetJavaCorePath(BufferRefC javaCorePath) {
		this->javaCorePath = javaCorePath;
	}
	inline BufferRefC GetJavaCorePath(Void) {
		return this->javaCorePath;
	}
private:
	Buffer packagePath;
	Set<SyntaxBuilderPtr> syntaxList;
	Set<PackagePtr> packageSet;
	Set<FileParserPtr> parserList;
	Buffer javaCorePath;
};

LAME_END2

#endif // ~__LAME_SCRIPT_PACKAGE__
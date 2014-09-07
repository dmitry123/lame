#ifndef __LAME_SCRIPT_PACKAGE__
#define __LAME_SCRIPT_PACKAGE__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API Package {
public:
	~Package();
public:
	Void Register(NodePtr node);
	PackagePtr Import(NodePtr node);
	Void Remove(PackagePtr package);
public:
	inline Set<PackagePtr>& GetPackages() {
		return this->packageSet;
	}
	inline Bool IsRegistered() {
		return !this->packagePath.empty();
	}
private:
	Buffer packagePath;
	Set<SyntaxBuilderPtr> syntaxList;
	Set<PackagePtr> packageSet;
	Set<FileParserPtr> parserList;
};

LAME_END2

#endif // ~__LAME_SCRIPT_PACKAGE__
#include "Package.h"
#include "FileParser.h"
#include "SyntaxBuilder.h"

#ifdef LAME_WINDOWS
#  include <Windows.h>
#  undef GetCurrentDirectory
#endif

LAME_BEGIN2(Script)

typedef Core::BufferRefC BufferRefC;

Package::~Package() {

	for (PackagePtr p : this->packageSet) {
		delete p;
	}

	for (FileParserPtr p : this->parserList) {
		delete p;
	}

	for (SyntaxBuilderPtr s : this->syntaxList) {
		delete s;
	}
}

Void Package::Register(NodePtr node) {

	FileParserPtr fileParser = node->GetBuilder()->GetParser();
	NodePtr nextNode = node->next;

	while (nextNode) {

		this->packagePath.append(nextNode->word);

		if (nextNode->next) {
			this->packagePath.append("/");
		}

		nextNode = nextNode->next;
	}

	List<Buffer> dirList = this->packagePath.Split('/');

	if (!File::ChangeDirectory(fileParser->GetPath().data())) {
		PostSyntaxError(node->lex->line, "Unable to find path (%s)",
			fileParser->GetPath().data());
	}

	for (BufferRefC b : dirList) {
		if (!File::ChangeDirectory("..")) {
			PostSyntaxError(node->lex->line, "Unable to find directory (%s)",
				b.data());
		}
	}

	this->packagePath = File::GetCurrentDirectory();
}

PackagePtr Package::Import(NodePtr node) {

	NodePtr nextNode = node->next;
	NodePtr classNode = NULL;
	Buffer filePath;

	this->node = node;

	while (nextNode) {

		if (nextNode->next) {
			filePath.append(nextNode->word + "/");
		}
		else {
			classNode = nextNode;
		}

		nextNode = nextNode->next;
	}

	char fullPath[4096];

#ifdef LAME_WINDOWS
	GetFullPathNameA(filePath.data(), sizeof(fullPath), fullPath, NULL);
#else
	char* realpath(char* path, char* dest);
#endif
	fullPath[strlen(fullPath) - filePath.length()] = '\0';

	if (classNode->lex->lex->id == kScriptLexMul) {

		List<Buffer> fileList;

		if (filePath.back() == '/') {
			filePath.pop_back();
		}

		try {
			fileList = Directory::GetFiles(filePath.data(),
				FALSE, "java&lame");
		}
		catch (...) {
			PostSyntaxError(node->lex->line, "Unable to open directory (%s)",
				filePath.data());
		}

		Buffer fileName = fullPath;

		for (BufferRefC file : fileList) {
			if (!this->_Import(fullPath + file) && !this->_Import(this->javaCorePath + file)) {
				PostSyntaxError(node->lex->line, "Unable to import file (%s)",
					fileName.data());
			}
		}
	}
	else {
		Buffer fileName = fullPath + filePath + classNode->word + ".java";
		if (!this->_Import(fileName) && !this->_Import(this->javaCorePath + filePath + classNode->word + ".java")) {
			PostSyntaxError(node->lex->line, "Unable to import file (%s)",
				fileName.data());
		}
	}

	return this;
}

Bool Package::_Import(BufferRefC fileName) {

	Uint64 hash = fileName.GetHash64();

	if (this->hashMap.count(hash)) {
		return TRUE;
	}
	this->hashMap.insert(hash);

	printf("[%s]\n", fileName.data());

	FileParserPtr fileParser = new FileParser();
	SyntaxBuilderPtr syntaxBuilder = new SyntaxBuilder();

	this->parserList.insert(fileParser);
	this->syntaxList.insert(syntaxBuilder);

	try {
		fileParser->Load(fileName);
	}
	catch (...) {
		return FALSE;
	}

	syntaxBuilder->Build(fileParser, this);

	auto rootList = syntaxBuilder->GetRootNode()->blockList;

	for (NodePtr n : rootList) {
		if (n->lex->lex->id == kScriptLexPackage) {
			continue;
		}
		node->parent->blockList.push_back(n);
	}

	return TRUE;
}

LAME_END2
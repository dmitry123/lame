#include "Package.h"
#include "FileParser.h"
#include "SyntaxBuilder.h"

LAME_BEGIN2(Script)

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

	while (nextNode) {

		if (nextNode->next) {
			filePath.append(nextNode->word + "/");
		}
		else {
			classNode = nextNode;
		}

		nextNode = nextNode->next;
	}

	if (classNode->lex->lex->id == kScriptLexMul) {

		List<Buffer> fileList;

		if (filePath.back() == '/') {
			filePath.pop_back();
		}

		try {
			fileList = Directory::GetFiles(filePath.data(),
				FALSE, "java");
		}
		catch (...) {
			PostSyntaxError(node->lex->line, "Unable to open directory (%s)",
				filePath.data());
		}

		for (BufferRefC file : fileList) {

			FileParserPtr fileParser = new FileParser();
			SyntaxBuilderPtr syntaxBuilder = new SyntaxBuilder();

			this->parserList.insert(fileParser);
			this->syntaxList.insert(syntaxBuilder);

			try {
				fileParser->Load(file);
			}
			catch (...) {
				PostSyntaxError(node->lex->line, "Unable to load file (%s)",
					file.data());
			}

			syntaxBuilder->Build(fileParser, this);

			for (NodePtr n : syntaxBuilder->GetRootNode()->blockList) {
				if (n->lex->lex->id == kScriptLexPackage) {
					continue;
				}
				node->parent->blockList.push_back(n);
			}
		}
	}
	else {
		FileParserPtr fileParser = new FileParser();
		SyntaxBuilderPtr syntaxBuilder = new SyntaxBuilder();

		this->parserList.insert(fileParser);
		this->syntaxList.insert(syntaxBuilder);

		Buffer fileName = filePath + classNode->word + ".java";

		try {
			fileParser->Load(fileName);
		} 
		catch (...) {
			PostSyntaxError(node->lex->line, "Unable to load file (%s)",
				fileName.data());
		}

		syntaxBuilder->Build(fileParser, this);

		for (NodePtr n : syntaxBuilder->GetRootNode()->blockList) {
			if (n->lex->lex->id == kScriptLexPackage) {
				continue;
			}
			node->parent->blockList.push_back(n);
		}
	}

	return NULL;
}

LAME_END2
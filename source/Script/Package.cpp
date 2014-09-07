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

	NodePtr nextNode = node->next;

	while (nextNode) {

		this->packagePath.append(nextNode->word);

		if (nextNode->next) {
			this->packagePath.append("/");
		}

		nextNode = nextNode->next;
	}
}

PackagePtr Package::Import(NodePtr node) {

	NodePtr nextNode = node->next;
	PackagePtr package = NULL;
	NodePtr classNode = NULL;

	while (nextNode) {

		if (nextNode->next) {
			this->packagePath.append(nextNode->word + "/");
		}
		else {
			classNode = nextNode;
		}

		nextNode = nextNode->next;
	}

	if (classNode->lex->lex->id == kScriptLexMul) {

		List<Buffer> fileList;

		if (this->packagePath.back() == '/') {
			this->packagePath.pop_back();
		}

		try {
			fileList = Directory::GetFiles(this->packagePath.data(),
				FALSE, "lame");
		}
		catch (...) {
			PostSyntaxError(node->lex->line, "Unable to load directory (%s)",
				this->packagePath.data());
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

		Buffer fileName = this->packagePath + classNode->word + ".lame";

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

Void Package::Remove(PackagePtr package) {

}

LAME_END2
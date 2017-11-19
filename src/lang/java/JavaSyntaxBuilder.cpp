#include "JavaSyntaxBuilder.h"

LAME_LANG_BEGIN_WITH(java)

void SyntaxBuilder::matchBuilder() {

	auto matcher = this->getMatcher();

	if (matcher->match(TokenType::IF)) {
		this->buildCondition();
	}
	else if (matcher->match(TokenType::ELSE)) {
		this->buildElse();
	}
	else if (matcher->match(TokenType::FOR)) {
		this->buildFor();
	}
	else if (matcher->match(TokenType::WHILE)) {
		this->buildWhile();
	}
	else if (matcher->match(TokenType::DO)) {
		this->buildDo();
	}
	else if (matcher->match(TokenType::CLASS)) {
		this->buildClass();
	}
	else if (matcher->match(TokenType::ANNOTATION)) {
		this->buildAnnotation();
	}
	else if (matcher->match(TokenType::INTERFACE)) {
		this->buildInterface();
	}
	else if (matcher->match({
		TokenType::IDENTIFIER,
		TokenType::LEFT_PARENTHESIS
	})) {
		this->buildMethod();
	}
	else if (matcher->match(TokenType::LEFT_BRACE)) {
		this->buildBody();
	}
	else {
		this->getActive()->insert(*this->begin);
	}
}

SyntaxBuilderPtr SyntaxBuilder::buildCondition() {

	auto root = this->push()
		->setFlags(Node::FLAG_CONDITION);

	this->require(TokenType::LEFT_PARENTHESIS)
		->buildArguments()
		->next()
		->require(TokenType::LEFT_BRACE)
		->buildBody();

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildElse() {

	if (this->getActive()->getLast() == nullptr || this->getActive()->getLast()->is(TokenType::IF) == false) {
		this->error(SyntaxError::ILLEGAL_PLACE);
	}

	auto root = this->push()
		->setFlags(Node::FLAG_ELSE);

	this->require(TokenType::LEFT_BRACE)
		->buildBody();

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildFor() {

	auto root = this->push()
		->setFlags(Node::FLAG_FOR);

	this->require(TokenType::LEFT_PARENTHESIS)
		->buildArguments()
		->next()
		->require(TokenType::LEFT_BRACE)
		->buildBody();

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildWhile() {
	
	auto root = this->push()
		->setFlags(Node::FLAG_WHILE);

	this->require(TokenType::LEFT_PARENTHESIS)
		->buildArguments()
		->next()
		->require(TokenType::LEFT_BRACE)
		->buildBody();

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildDo() {

	auto root = this->push()
		->setFlags(Node::FLAG_DO);

	this->require(TokenType::LEFT_BRACE)
		->buildBody()
		->require(TokenType::WHILE)
		->next()
		->buildWhile();

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildClass() {

	auto root = this->push()
		->setFlags(Node::FLAG_CLASS);

	auto& i = this->begin;

	this->require(TokenType::IDENTIFIER)->getActive()
		->setIdentifier(*i);

	this->next();

	if ((*i)->is(TokenType::EXTENDS)) {
		this->buildExtends();
	}

	if ((*i)->is(TokenType::IMPLEMENTS)) {
		this->buildImplements();
	}

	return this->buildBody()
		->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildInterface() {

	auto root = this->push()
		->setFlags(Node::FLAG_INTERFACE);

	auto& i = this->begin;

	this->require(TokenType::IDENTIFIER)->getActive()
		->setIdentifier(*i);

	this->next();

	if ((*i)->is(TokenType::EXTENDS)) {
		this->buildExtends();
	}

	return this->buildBody()
		->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildAnnotation() {
	
	auto root = this->push()
		->setFlags(Node::FLAG_ANNOTATION);

	auto& i = this->begin;

	this->require(TokenType::INTERFACE)
		->next()
		->require(TokenType::IDENTIFIER);

	root->setIdentifier(*i);
	this->next();

	if ((*i)->is(TokenType::EXTENDS)) {
		this->buildExtends()->next();
	}

	if ((*i)->is(TokenType::IMPLEMENTS)) {
		this->buildImplements();
	}

	return this->buildBody()
		->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildMethod() {

	auto root = this->push()
		->setFlags(Node::FLAG_METHOD);

	auto& i = this->begin;

	this->prev()
		->require(TokenType::LEFT_PARENTHESIS)
		->buildArguments()
		->next();

	if ((*i)->is(TokenType::THROWS)) {
		this->buildThrows();
	}

	if ((*i)->is(TokenType::LEFT_BRACE)) {
		this->buildBody();
	}
	else {
		this->require(TokenType::SEMICOLON);
	}

	return this->pop(root);
}

SyntaxBuilderPtr SyntaxBuilder::buildArguments() {

	this->require(TokenType::LEFT_PARENTHESIS)
		->next();

	auto root = this->push(this->getActive()->require(this->getTemp()))
		->setFlags(Node::FLAG_TEMP);

	auto& i = this->begin;

	for (; this->finish(i) && !(*i)->is(TokenType::RIGHT_PARENTHESIS); i++) {
		this->matchBuilder();
	}

	this->pop(root);

	for (auto n : root->getChildren()) {
		this->getActive()->getArgs().push_back(n);
	}

	root->erase();

	return this;
}

SyntaxBuilderPtr SyntaxBuilder::buildBody() {

	this->require(TokenType::LEFT_BRACE)
		->next();

	auto root = this->push(this->getActive()->require(this->getTemp()))
		->setFlags(Node::FLAG_TEMP);

	auto& i = this->begin;

	for (; this->finish(i) && !(*i)->is(TokenType::RIGHT_BRACE) && this->finish(i); i++) {
		this->matchBuilder();
	}

	this->pop(root);

	for (auto n : root->getChildren()) {
		this->getActive()->getBody().push_back(n);
	}

	root->erase();

	return this;
}

SyntaxBuilderPtr SyntaxBuilder::buildImplements() {

	this->require(TokenType::IMPLEMENTS)
		->next();

	auto root = this->push(this->getActive()->require(this->getTemp()))
		->setFlags(Node::FLAG_TEMP);

	for (auto& i = this->begin; this->finish(i) && !(*i)->is(TokenType::LEFT_BRACE); i++) {
		this->matchBuilder();
	}

	this->pop(root);

	for (auto n : root->getChildren()) {
		this->getActive()->getImplements().push_back(n);
	}

	root->erase();

	return this;
}

SyntaxBuilderPtr SyntaxBuilder::buildExtends() {

	this->require(TokenType::EXTENDS)
		->next();

	auto root = this->push(this->getActive()->require(this->getTemp()))
		->setFlags(Node::FLAG_TEMP);

	for (auto& i = this->begin; this->finish(i); i++) {

		if ((*i)->is(TokenType::IMPLEMENTS) ||
			(*i)->is(TokenType::LEFT_BRACE)
		) {
			break;
		}

		this->matchBuilder();
	}

	this->pop(root);

	for (auto n : root->getChildren()) {
		this->getActive()->getExtends().push_back(n);
	}

	root->erase();

	return this;
}

SyntaxBuilderPtr SyntaxBuilder::buildThrows() {

	this->require(TokenType::THROWS)
		->next();

	auto root = this->push(this->getActive()->require(this->getTemp()))
		->setFlags(Node::FLAG_TEMP);

	for (auto& i = this->begin; this->finish(i); i++) {

		if ((*i)->is(TokenType::SEMICOLON) ||
			(*i)->is(TokenType::LEFT_BRACE)
		) {
			break;
		}

		this->matchBuilder();
	}

	this->pop(root);

	for (auto n : root->getChildren()) {
		this->getActive()->getThrows().push_back(n);
	}

	root->erase();

	return this;
}

LAME_LANG_END_WITH
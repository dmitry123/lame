#include "JavaScopeBuilder.h"
#include "JavaTokenManager.h"

LAME_LANG_BEGIN_WITH(java)

void ScopeBuilder::matchBuilder(NodePtr node) {

	if (node->is(Node::FLAG_CLASS) ||
		node->is(Node::FLAG_INTERFACE) ||
		node->is(Node::FLAG_ANNOTATION)
	) {
		this->buildClass(node);
	}
	else if (node->is(Node::FLAG_METHOD)) {
		this->buildMethod(node);
	}
	else if (
		node->is(Node::FLAG_CONDITION) ||
		node->is(Node::FLAG_ELSE) ||
		node->is(Node::FLAG_FOR) ||
		node->is(Node::FLAG_WHILE) ||
		node->is(Node::FLAG_DO)
	) {
		this->buildConstruction(node);
	}
	else {
		this->buildExpression(node);
	}
}

void ScopeBuilder::buildConstruction(NodePtr node) {

	ObjectPtr obj = this->push(node)->setType(Object::TYPE_CLASS)
		->setModifiers(this->getModifiers());

	this->build(obj->getNode()->getBody());
	this->expression.clear();

	this->build(obj->getNode()->getArgs());
	this->expression.clear();

	this->pop(obj);
}

void ScopeBuilder::buildClass(NodePtr node) {

	ObjectPtr obj = this->push(node)->setType(Object::TYPE_CLASS)
		->setModifiers(this->getModifiers());

	this->build(obj->getNode()->getExtends());
	this->expression.clear();

	this->build(obj->getNode()->getImplements());
	this->expression.clear();

	this->build(obj->getNode()->getBody());
	this->expression.clear();

	this->pop(obj);

	this->getManager()->declare(obj);
}

void ScopeBuilder::buildExpression(NodePtr node) {

	if (node->getToken()->isModifier()) {
		this->setModifiers(node->getToken()->id);
	}
	else {
		this->expression.push_back(node);
	}
}

void ScopeBuilder::buildMethod(NodePtr node) {

	auto type = this->getManager()->declare(
		this->read()
	);

	if (this->expression.size() != 0) {
		//throw new SyntaxError(SyntaxError::ILLEGAL_PLACE, this->expression.back()->getLine());
	}

	auto obj = this->push(node)->setType(Object::TYPE_METHOD)
		->setModifiers(this->getModifiers())->setAttached(type);

	this->build(node->getArgs());
	this->expression.clear();

	this->build(node->getBody());
	this->expression.clear();

	this->pop(obj);
}

UInt32 ScopeBuilder::setModifiers(UInt32 id) {

	switch (TokenType(id)) {
	case TokenType::PUBLIC:
		return (this->modifiers |= Object::MODIFIER_PUBLIC);
	case TokenType::PROTECTED:
		return (this->modifiers |= Object::MODIFIER_PROTECTED);
	case TokenType::PRIVATE:
		return (this->modifiers |= Object::MODIFIER_PRIVATE);
	case TokenType::FINAL:
		return (this->modifiers |= Object::MODIFIER_FINAL);
	case TokenType::NATIVE:
		return (this->modifiers |= Object::MODIFIER_NATIVE);
	case TokenType::STATIC:
		return (this->modifiers |= Object::MODIFIER_STATIC);
	default:
		return this->modifiers;
	}
}

UInt32 ScopeBuilder::getModifiers() {

	UInt32 modifiers = this->modifiers;
	this->modifiers = 0;
	return modifiers;
}

LAME_LANG_END_WITH
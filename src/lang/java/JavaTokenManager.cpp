#include "JavaTokenManager.h"

#define _TOKEN_0(id, word) \
	{ id, { SInt32(id), word, 0, 0, 0} }

#define _TOKEN_1(id, word, flags) \
	{ id, { SInt32(id), word, 0, flags, 0} }

#define _TOKEN_4(id, word, args, precedence, flags) \
	{ id, { SInt32(id), word, precedence, Token::FLAG_OPERATOR | flags, UInt32(args) } }

#define _TOKEN_M(id, word, args, precedence, flags) \
	_TOKEN_4(id, word, args, precedence, Token::FLAG_MATH | flags)

#define _TOKEN_B(id, word, args, precedence, flags) \
	_TOKEN_4(id, word, args, precedence, Token::FLAG_BOOLEAN | flags)

#define _TOKEN_K(id, word, flags) \
	_TOKEN_4(id, word, 0, 0, Token::FLAG_KEYWORD | flags)

LAME_LANG_BEGIN_WITH(java)

TokenManager::TokenMap javaTokenMap = {
	/* Defaults */
	_TOKEN_0(TokenType::IDENTIFIER, "<identifier>"),
	/* Type Constants */
	_TOKEN_1(TokenType::CONST_INTEGER, "<integer>", Token::FLAG_CONSTANT),
	_TOKEN_1(TokenType::CONST_FLOAT, "<float>", Token::FLAG_CONSTANT),
	_TOKEN_1(TokenType::CONST_STRING, "<string>", Token::FLAG_CONSTANT),
	_TOKEN_1(TokenType::CONST_CHAR, "<char>", Token::FLAG_CONSTANT),
	/* Brackets & Braces */
	_TOKEN_0(TokenType::RIGHT_BRACKET, "]"),
	_TOKEN_0(TokenType::LEFT_BRACKET, "["),
	_TOKEN_0(TokenType::RIGHT_PARENTHESIS, ")"),
	_TOKEN_0(TokenType::LEFT_PARENTHESIS, "("),
	_TOKEN_0(TokenType::RIGHT_BRACE, "}"),
	_TOKEN_0(TokenType::LEFT_BRACE, "{"),
	/* Language Specials */
	_TOKEN_0(TokenType::ARGUMENT_LIST, "..."),
	/* Operators */
	_TOKEN_4(TokenType::ARRAY_INDEX, "<index>", 1, 1, Token::FLAG_LEFT),
	_TOKEN_4(TokenType::METHOD_CALL, "<call>", -1, 1, Token::FLAG_LEFT),
	_TOKEN_4(TokenType::MEMBER_ACCESS, ".", 2, 1, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::POSTFIX_INCREMENT, "<inc>", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::PREFIX_INCREMENT, "++", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::POSTFIX_DECREMENT, "<dec>", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::PREFIX_DECREMENT, "--", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::UNARY_PLUS, "<plus>", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::UNARY_MINUS, "<minus>", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::BITWISE_NOT, "~", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::BOOLEAN_NOT, "!", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_4(TokenType::TYPE_CAST, "<cast>", 1, 2, Token::FLAG_RIGHT),
	_TOKEN_4(TokenType::OBJECT_CREATION, "new", 2, 2, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::MULTIPLICATION, "*", 2, 3, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::DIVISION, "/", 2, 3, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::REMAINDER, "%", 2, 3, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::ADDITION, "+", 2, 4, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::SUBSTRACTION, "-", 2, 4, Token::FLAG_LEFT),
	_TOKEN_4(TokenType::STRING_CONCATENATION, "<concat>", 2, 4, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::UNSIGNED_BIT_SHIFT_RIGHT, ">>", 2, 5, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::SIGNED_BIT_SHIFT_LEFT, "<<", 2, 5, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::SIGNED_BIT_SHIFT_RIGHT, ">>>", 2, 5, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::LESS_THAN, "<", 2, 6, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::LESS_THAN_OR_EQUAL, "<=", 2, 6, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::GREATER_THEN, ">", 2, 6, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::GREATER_THEN_OR_EQUAL, ">=", 2, 6, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::INSTANCE_OF, "instanceof", 2, 6, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::EQUAL, "==", 2, 7, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::NOT_EQUAL, "!=", 2, 7, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::BITWISE_AND, "&", 2, 8, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::BITWISE_XOR, "^", 2, 9, Token::FLAG_LEFT),
	_TOKEN_M(TokenType::BITWISE_OR, "|", 2, 10, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::BOOLEAN_OR, "||", 2, 11, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::BOOLEAN_AND, "&&", 2, 12, Token::FLAG_LEFT),
	_TOKEN_B(TokenType::CONDITIONAL, "<ternary>", 3, 13, Token::FLAG_RIGHT),
	_TOKEN_4(TokenType::ASSIGNMENT, "=", 2, 14, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::MULTIPLICATION_ASSIGNMENT, "*=", 2, 3, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::DIVISION_ASSIGNMENT, "/=", 2, 3, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::REMAINDER_ASSIGNMENT, "%=", 2, 3, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::ADDITION_ASSIGNMENT, "+=", 2, 4, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::SUBSTRACTION_ASSIGNMENT, "-=", 2, 4, Token::FLAG_RIGHT),
	_TOKEN_4(TokenType::STRING_CONCATENATION_ASSIGNMENT, "<concat>", 2, 4, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::UNSIGNED_BIT_SHIFT_RIGHT_ASSIGNMENT, ">>=", 2, 5, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::SIGNED_BIT_SHIFT_LEFT_ASSIGNMENT, "<<=", 2, 5, Token::FLAG_RIGHT),
	_TOKEN_M(TokenType::SIGNED_BIT_SHIFT_RIGHT_ASSIGNMENT, ">>>=", 2, 5, Token::FLAG_RIGHT),
	_TOKEN_4(TokenType::RETURN, "return", 1, 6, Token::FLAG_OPERATOR),
	/* Language Construction & Keywords */
	_TOKEN_K(TokenType::IF, "if", 0),
	_TOKEN_K(TokenType::ELSE, "else", 0),
	_TOKEN_K(TokenType::WHILE, "while", 0),
	_TOKEN_K(TokenType::DO, "do", 0),
	_TOKEN_K(TokenType::FOR, "for", 0),
	_TOKEN_K(TokenType::TRY, "try", 0),
	_TOKEN_K(TokenType::CATCH, "catch", 0),
	_TOKEN_K(TokenType::FINALLY, "finally", 0),
	_TOKEN_K(TokenType::THROW, "throw", 0),
	_TOKEN_K(TokenType::THROWS, "throws", 0),
	_TOKEN_K(TokenType::SWITCH, "switch", 0),
	_TOKEN_K(TokenType::CLASS, "class", 0),
	_TOKEN_K(TokenType::ENUM, "enum", 0),
	_TOKEN_K(TokenType::INTERFACE, "interface", 0),
	_TOKEN_K(TokenType::ANNOTATION, "@", 0),
	_TOKEN_K(TokenType::IMPLEMENTS, "implements", 0),
	_TOKEN_K(TokenType::EXTENDS, "extends", 0),
	_TOKEN_K(TokenType::BREAK, "break", 0),
	_TOKEN_K(TokenType::CONTINUE, "continue", 0),
	_TOKEN_K(TokenType::PUBLIC, "public", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::PRIVATE, "private", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::PROTECTED, "protected", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::STATIC, "static", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::NATIVE, "native", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::FINAL, "final", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::ABSTRACT, "abstract", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::SYNCHRONIZED, "synchonized", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::TRASIENT, "transient", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::VOLATILE, "volatile", Token::FLAG_MODIFIER),
	_TOKEN_K(TokenType::DEFAULT, "default", 0),
	_TOKEN_K(TokenType::CASE, "case", 0),
	_TOKEN_K(TokenType::PACKAGE, "package", 0),
	_TOKEN_K(TokenType::IMPORT, "import", 0),
	_TOKEN_K(TokenType::GOTO, "goto", 0),
	_TOKEN_K(TokenType::THIS, "this", 0),
	_TOKEN_K(TokenType::SUPER, "super", 0),
	/* Others */
	_TOKEN_4(TokenType::COMMA, ",", 2, 6, Token::FLAG_OPERATOR),
	_TOKEN_0(TokenType::LEFT_COMMENT, "/*"),
	_TOKEN_0(TokenType::RIGHT_COMMENT, "*/"),
	_TOKEN_0(TokenType::LINE_COMMENT, "//"),
	_TOKEN_0(TokenType::QUOTE, "\""),
	_TOKEN_0(TokenType::APOSTROPHE, "'"),
	_TOKEN_4(TokenType::SEMICOLON, ";", 0, 6, Token::FLAG_OPERATOR),
	_TOKEN_0(TokenType::COLON, ":"),
	/* Temporary */
	_TOKEN_0(TokenType::_TEMP, "<temp>")
};

TokenManagerPtr manager = 0;

::lame::lang::TokenManager<TokenType>* TokenManager::getManager() {
	if (manager == nullptr) {
		return (manager = new TokenManager(javaTokenMap));
	}
	else {
		return manager;
	}
}

LAME_LANG_END_WITH
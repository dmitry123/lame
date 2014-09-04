#ifndef __LAME_SCRIPT_SYNTAX_ERROR__
#define __LAME_SCRIPT_SYNTAX_ERROR__

#include "Define.h"

LAME_BEGIN2(Script)

class LAME_API SyntaxError {
public:
	enum class Code : Uint32 {
		ExceptedIdentifier,
		UnexceptedIdentifier,
		ObjectNotVariable,
		ClassHasAlreadyBeenExtended,
		InterfaceHasAlreadyImplementedIt,
		CantImplementNonInterface,
		ClassHasAlreadyImplementedIt,
		ClassCanOnlyImplementInterfaces,
		InterfaceCantImplementMethods,
		ClassMustImplementInterface,
		ClassCantImplementAbstractMethods,
		ClassMustImplementAbstractMethods,
		QuoteOrApostropheMismacthed,
		ParenthesesMismatched,
		BracesMismatched,
		NotFinishedExpression,
		ModificatorInIllegalPlace,
		LostLeftBrace,
		TwoSimilarParenthesis,
		IllegalToken,
		LostLeftParenthesis,
		BracesOrParenthesesMismatched,
		LostSemicolon,
		CantCreateClassWithoutName
	};
public:

private:

};

LAME_END2

#endif // ~__LAME_SCRIPT_SYNTAX_ERROR__
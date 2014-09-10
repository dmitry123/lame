#include "LittleCalculator.h"
#include "Variable.h"

LAME_BEGIN2(Script)

LittleCalculator::~LittleCalculator() {

	for (NodePtr n : this->nodeList) {
		delete n;
	}
}

Bool LittleCalculator::Compute(NodePtr node, ObjectPtr left, ObjectPtr right) {

	ObjectPtr newVar;
	Bool result;

	if (!left) {
		return FALSE;
	}

	if (left && !left->CheckType(Object::Type::Variable)) {
		PostSyntaxError(node->lex->line, "Object must be variable (%s)",
			left->GetName().data());
	}

	if (right && !right->CheckType(Object::Type::Variable)) {
		PostSyntaxError(node->lex->line, "Object must be variable (%s)",
			right->GetName().data());
	}

	this->currentNode = node;

	do {
		newVar = left->Clone("", ObjectPtr(left->GetParent()))->GetVariable();
	} while (!left->Root()->Add(newVar));

	if (left && right) {
		result = this->_Binary(newVar->GetVariable(), right->GetVariable());
	}
	else {
		result = this->_Unary(newVar->GetVariable());
	}

	if (!result) {
		return FALSE;
	}

	Buffer newName;

	if (newVar->GetClass()->IsInt()) {
		newName.Format("%lld",
			newVar->GetVariable()->v.intValue);
	}
	else if (newVar->GetClass()->IsFloat()) {
		newName.Format("%.4f",
			newVar->GetVariable()->v.intValue);
	}
	else if (newVar->GetClass()->IsString()) {
		newName = newVar->GetVariable()->stringValue;
	}

	NodePtr newNode = new Node(
		*left->GetNode());

	newVar->SetNode(newNode);
	newVar->GetNode()->word = newName;

	this->nodeList.push_back(newNode);
	left->SetNode(newNode);
	newNode->var = newVar;

	return result;
}

Bool LittleCalculator::_Unary(VariablePtr left) {

	Bool isFloat = left->GetClass()->IsFloatLike();
	Bool isInt = left->GetClass()->IsIntegerLike();

	switch (this->GetNode()->lex->lex->id) {
	case kScriptLexBitNot:
		if (isInt) {
			left->v.intValue = ~left->v.intValue;
		} else if (isFloat) {
			return FALSE;
		} else {
			return FALSE;
		}
		break;
	case kScriptLexNot:
		if (isInt) {
			left->v.intValue = !left->v.intValue;
		} else if (isFloat) {
			left->v.floatValue = !left->v.floatValue;
		} else {
			return FALSE;
		}
		break;
	case kScriptLexUnaryMinus:
	case kScriptLexSub:
		if (isInt) {
			left->v.intValue = -left->v.intValue;
		} else if (isFloat) {
			left->v.floatValue = -left->v.floatValue;
		} else {
			return FALSE;
		}
		break;
	case kScriptLexUnaryPlus:
	case kScriptLexAdd:
		if (isInt) {
			left->v.intValue = +left->v.intValue;
		} else if (isFloat) {
			left->v.floatValue = +left->v.floatValue;
		} else {
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

Bool LittleCalculator::_Binary(VariablePtr left, VariablePtr right) {

	Bool isFloat = left->GetClass()->IsFloatLike();
	Bool isInt = left->GetClass()->IsIntegerLike();
	Bool isString = left->GetClass()->IsString();

	if (!isFloat && !isInt && !isString) {
		return FALSE;
	}

	switch (this->GetNode()->lex->lex->id) {
	case kScriptLexMul:
		if (isFloat) {
			left->v.floatValue *= right->GetFloat();
		} else if (isInt) {
			left->v.intValue *= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexDiv:
		if (isFloat) {
			left->v.floatValue /= right->GetFloat();
		} else if (isInt) {
			left->v.intValue /= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexMod:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue %= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexAdd:
		if (isFloat) {
			left->v.floatValue += right->GetFloat();
		} else if (isInt) {
			left->v.intValue += right->GetInteger();
		} else {
			left->stringValue.append(right->GetString());
		}
		break;
	case kScriptLexSub:
		if (isFloat) {
			left->v.floatValue -= right->GetFloat();
		} else if (isInt) {
			left->v.intValue -= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitShiftL:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue <<= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitShiftR:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue >>= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitShiftU:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue >>= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitAnd:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue &= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitXor:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue ^= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBitOr:
		if (isFloat) {
			return FALSE;
		} else if (isInt) {
			left->v.intValue |= right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexBelow:
		if (isFloat) {
			left->v.intValue = left->v.floatValue < right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue < right->GetInteger();
		} else {
			left->v.intValue = left->stringValue < right->GetString();
		}
		break;
	case kScriptLexAbove:
		if (isFloat) {
			left->v.intValue = left->v.floatValue > right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue > right->GetInteger();
		} else {
			left->v.intValue = left->stringValue > right->GetString();
		}
		break;
	case kScriptLexBelowEqual:
		if (isFloat) {
			left->v.intValue = left->v.floatValue <= right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue <= right->GetInteger();
		} else {
			left->v.intValue = left->stringValue <= right->GetString();
		}
		break;
	case kScriptLexAboveEqual:
		if (isFloat) {
			left->v.intValue = left->v.floatValue >= right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue >= right->GetInteger();
		} else {
			left->v.intValue = left->stringValue >= right->GetString();
		}
		break;
	case kScriptLexEqual:
		if (isFloat) {
			left->v.intValue = left->v.floatValue == right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue == right->GetInteger();
		} else {
			left->v.intValue = left->stringValue == right->GetString();
		}
		break;
	case kScriptLexNotEqual:
		if (isFloat) {
			left->v.intValue = left->v.floatValue != right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue != right->GetInteger();
		} else {
			left->v.intValue = left->stringValue != right->GetString();
		}
		break;
	case kScriptLexAnd:
		if (isFloat) {
			left->v.intValue = left->v.floatValue && right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue && right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	case kScriptLexOr:
		if (isFloat) {
			left->v.intValue = left->v.floatValue || right->GetFloat();
		} else if (isInt) {
			left->v.intValue = left->v.intValue || right->GetInteger();
		} else {
			return FALSE;
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

LAME_END2
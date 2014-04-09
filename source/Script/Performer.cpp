#include "Script.h"

LAME_BEGIN

#define __EvalError(_operation) \
	PostSyntaxError(left.object->line, "Unable to apply %s operation to this type", #_operation, left.object->type.GetString());

Void ScriptPerformer::Set(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = right.boolValue;
		break;
	case kScriptTypeFloat:
		left.floatValue = right.floatValue;
		break;
	case kScriptTypeInt:
		left.intValue = right.intValue;
		break;
	case kScriptTypeString:
		left.stringValue = right.stringValue;
		break;
	default:
		__EvalError(=);
	}
}

Void ScriptPerformer::Add(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue += right.boolValue;
		break;
	case kScriptTypeFloat:
		left.floatValue += right.floatValue;
		break;
	case kScriptTypeInt:
		left.intValue += right.intValue;
		break;
	case kScriptTypeString:
		left.stringValue += right.stringValue;
		break;
	default:
		__EvalError(+);
	}
}

Void ScriptPerformer::Sub(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue -= right.boolValue;
		break;
	case kScriptTypeFloat:
		left.floatValue -= right.floatValue;
		break;
	case kScriptTypeInt:
		left.intValue -= right.intValue;
		break;
	default:
		__EvalError(-);
	}
}

Void ScriptPerformer::Mul(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue *= right.boolValue;
		break;
	case kScriptTypeFloat:
		left.floatValue *= right.floatValue;
		break;
	case kScriptTypeInt:
		left.intValue *= right.intValue;
		break;
	default:
		__EvalError(*);
	}
}

Void ScriptPerformer::Div(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue /= right.boolValue;
		break;
	case kScriptTypeFloat:
		left.floatValue /= right.floatValue;
		break;
	case kScriptTypeInt:
		left.intValue /= right.intValue;
		break;
	default:
		__EvalError(/ );
	}
}

Void ScriptPerformer::Mod(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue %= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue %= right.intValue;
		break;
	default:
		__EvalError(%);
	}
}

Void ScriptPerformer::BitAnd(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue &= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue &= right.intValue;
		break;
	default:
		__EvalError(&);
	}
}

Void ScriptPerformer::BitOr(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue |= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue |= right.intValue;
		break;
	default:
		__EvalError(|);
	}
}

Void ScriptPerformer::BitXor(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue ^= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue ^= right.intValue;
		break;
	default:
		__EvalError(^);
	}
}

Void ScriptPerformer::BitShiftR(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue >>= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue >>= right.intValue;
		break;
	default:
		__EvalError(>>);
	}
}

Void ScriptPerformer::BitShiftL(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue <<= right.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue <<= right.intValue;
		break;
	default:
		__EvalError(<<);
	}
}

Void ScriptPerformer::And(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue && right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue && right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue && right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue.length() && right.stringValue.length();
		break;
	default:
		__EvalError(&&);
	}
}

Void ScriptPerformer::Or(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue || right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue || right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue || right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue.length() || right.stringValue.length();
		break;
	default:
		__EvalError(||);
	}
}

Void ScriptPerformer::Above(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue > right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue > right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue > right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue > right.stringValue;
		break;
	default:
		__EvalError(>);
	}
}

Void ScriptPerformer::Bellow(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue < right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue < right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue < right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue < right.stringValue;
		break;
	default:
		__EvalError(<);
	}
}

Void ScriptPerformer::AboveEqual(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue >= right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue >= right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue >= right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue >= right.stringValue;
		break;
	default:
		__EvalError(>=);
	}
}

Void ScriptPerformer::BellowEqual(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue <= right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue <= right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue <= right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue <= right.stringValue;
		break;
	default:
		__EvalError(<=);
	}
}

Void ScriptPerformer::Equal(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue == right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue == right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue == right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue == right.stringValue;
		break;
	default:
		__EvalError(==);
	}
}

Void ScriptPerformer::NotEqual(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = left.boolValue != right.boolValue;
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue != right.floatValue;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue != right.intValue;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue != right.stringValue;
		break;
	default:
		__EvalError(!=);
	}
}

Void ScriptPerformer::BitNot(ScriptVariable& left) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = ~left.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue = ~left.intValue;
		break;
	default:
		__EvalError(~);
	}
}

Void ScriptPerformer::Not(ScriptVariable& left) {
	switch (left.object->type) {
	case kScriptTypeBool:
		left.boolValue = !left.boolValue;
		break;
	case kScriptTypeInt:
		left.intValue = !left.intValue;
		break;
	default:
		__EvalError(!);
	}
}

Void ScriptPerformer::Inc(ScriptVariable& left) {
	switch (left.object->type) {
	case kScriptTypeBool:
		++left.boolValue;
		break;
	case kScriptTypeInt:
		++left.intValue;
		break;
	default:
		__EvalError(++);
	}
}

Void ScriptPerformer::Dec(ScriptVariable& left) {
	switch (left.object->type) {
	case kScriptTypeBool:
		--left.boolValue;
		break;
	case kScriptTypeInt:
		--left.intValue;
		break;
	default:
		__EvalError(--);
	}
}

Void ScriptPerformer::AsBool(ScriptVariable& left) {
	switch (left.object->type) {
	case kScriptTypeBool:
		break;
	case kScriptTypeFloat:
		left.boolValue = left.floatValue != 0;
		break;
	case kScriptTypeInt:
		left.boolValue = left.intValue != 0;
		break;
	case kScriptTypeString:
		left.boolValue = left.stringValue.length() != 0;
		break;
	default:
		left.boolValue = 0;
		break;
	}
}

Bool ScriptPerformer::RegisterType(ScriptNodePtr node) {
	this->typeList.push_back(node);
	return LAME_TRUE;
}

Bool ScriptPerformer::RegisterVar(ScriptNodePtr node) {
	if (!node->var) {
		node->var = new ScriptVariable(node->object);
	}
	this->varList.push_back(node);
	return LAME_TRUE;
}

ScriptNodePtr ScriptPerformer::FindType(StringC name) {
	for (ScriptNodePtr node : this->typeList) {
		if (node->object->word == name) {
			return node;
		}
	}
	return LAME_NULL;
}

ScriptNodePtr ScriptPerformer::FindVar(StringC name) {
	for (ScriptNodePtr node : this->varList) {
		if (node->object->word == name) {
			return node;
		}
	}
	return LAME_NULL;
}

Void ScriptPerformer::EvaluateSingleExpression(
	ScriptNodePtr left,
	ScriptNodePtr token
	) {
	if (!left->var && token->object->object != kScriptObjectType) {
		ScriptNodePtr left_ = FindVar(left->object->word.data());
		if (!this->FindVar(left->object->word.data())) {
			PostSyntaxError(left->object->line, "Undeclared variable (%s)", left->object->word.data());
		}
		if (token->object->IsRightAssociated()) {
			this->tempList.push_back(*left_->var);
			left->var = &this->tempList.back();
		}
		else {
			left->object = left_->object;
			left->var = left_->var;
		}
	}
	switch (token->object->object) {
	case kScriptObjectType:
		if (!this->RegisterVar(left)) {
			PostSyntaxError(left->object->line, "Variable redeclaration (%s)", left->object->word.data());
		}
		left->object->type = token->object->type;
		break;
	case kScriptObjectIncrement:
		this->Inc(*left->var);
		break;
	case kScriptObjectDecrement:
		this->Dec(*left->var);
		break;
	case kScriptObjectNot:
		this->Not(*left->var);
		break;
	case kScriptObjectBitNot:
		this->BitNot(*left->var);
		break;
	default:
		break;
	}
}

Void ScriptPerformer::EvaluateDoubleExpression(
	ScriptNodePtr left,
	ScriptNodePtr right,
	ScriptNodePtr token
	) {
	Bool isUseVar = LAME_FALSE;

	if (token->object->IsRightAssociated()) {
		if (left->object->object == kScriptObjectVariable) {
			left->var = LAME_NULL;
		}
	}

	if (!left->var) {
		ScriptNodePtr left_ = this->FindVar(left->object->word.data());
		if (!left_) {
			PostSyntaxError(left->object->line, "Undeclared variable (%s)", left->object->word.data());
		}
		if (token->object->IsRightAssociated()) {
			this->tempList.push_back(*left_->var);
			left->var = &this->tempList.back();
		}
		else {
			left->object = left_->object;
			left->var = left_->var;
		}
		isUseVar = LAME_TRUE;
	}
	if (!right->var) {
		ScriptNodePtr right_ = this->FindVar(right->object->word.data());
		if (!right_) {
			PostSyntaxError(right->object->line, "Undeclared variable (%s)", right->object->word.data());
		}
		this->tempList.push_back(*right_->var);
		right->var = &this->tempList.back();
	}

	switch (token->object->object) {
	case kScriptObjectAnd:
		this->And(*left->var, *right->var);
		break;
	case kScriptObjectOr:
		this->Or(*left->var, *right->var);
		break;
	case kScriptObjectAbove:
		this->Above(*left->var, *right->var);
		break;
	case kScriptObjectAboveEqual:
		this->AboveEqual(*left->var, *right->var);
		break;
	case kScriptObjectBellow:
		this->Bellow(*left->var, *right->var);
		break;
	case kScriptObjectBellowEqual:
		this->BellowEqual(*left->var, *right->var);
		break;
	case kScriptObjectEqual:
		this->Equal(*left->var, *right->var);
		break;
	case kScriptObjectNotEqual:
		this->NotEqual(*left->var, *right->var);
		break;
	default:
		goto __EvaluateMathExpression;
	}
	left->object->type = kScriptTypeBool;
	isUseVar = LAME_FALSE;

	goto __Return;
__EvaluateMathExpression:

	ScriptVariable::Convert(*right->var, *left->var);

	switch (token->object->object) {
	case kScriptObjectSet:
		this->Set(*left->var, *right->var);
		break;
	case kScriptObjectMul:
	case kScriptObjectMulSet:
		this->Mul(*left->var, *right->var);
		break;
	case kScriptObjectDiv:
	case kScriptObjectDivSet:
		this->Div(*left->var, *right->var);
		break;
	case kScriptObjectMod:
	case kScriptObjectModSet:
		this->Mod(*left->var, *right->var);
		break;
	case kScriptObjectAdd:
	case kScriptObjectAddSet:
		this->Add(*left->var, *right->var);
		break;
	case kScriptObjectSub:
	case kScriptObjectSubSet:
		this->Sub(*left->var, *right->var);
		break;
	case kScriptObjectBitShiftL:
	case kScriptObjectBitShiftSetL:
		this->BitShiftL(*left->var, *right->var);
		break;
	case kScriptObjectBitShiftR:
	case kScriptObjectBitShiftSetR:
		this->BitShiftR(*left->var, *right->var);
		break;
	case kScriptObjectBitAnd:
	case kScriptObjectBitAndSet:
		this->BitAnd(*left->var, *right->var);
		break;
	case kScriptObjectBitXor:
	case kScriptObjectBitXorSet:
		this->BitXor(*left->var, *right->var);
		break;
	case kScriptObjectBitOr:
	case kScriptObjectBitOrSet:
		this->BitOr(*left->var, *right->var);
		break;
	default:
		break;
	}

__Return:
	return;
}

Void ScriptPerformer::Evaluate(
	Vector<ScriptNodePtr>* list,
	Vector<ScriptNodePtr>* result
) {
	typedef Vector<ScriptNodePtr>::iterator Iterator;

	Uint32 arguments = 0;
	ScriptNodePtr left = 0;
	ScriptNodePtr right = 0;
	ScriptNodePtr kw = 0;

	this->tempList.clear();
	for (Iterator i = list->begin(); i != list->end(); i++) {

		kw = *i;

		if (kw->object->object == kScriptObjectVariable) {

			ScriptType type;
			StringC name = kw->object->word.data();

			type.Parse(&name);

			if (type == kScriptTypeDefault) {
				ScriptNodePtr typeNode = this->FindType(kw->object->word.data());
				if (typeNode) {
					kw = typeNode;
					kw->object->object = kScriptObjectType;
				}
			}
			else {
				kw->object->type = type;
				kw->object->object = kScriptObjectType;
			}
		}

		if (kw->object->object == kScriptObjectInt ||
			kw->object->object == kScriptObjectFloat ||
			kw->object->object == kScriptObjectString ||
			kw->object->object == kScriptObjectDefault ||
			kw->object->object == kScriptObjectVariable
		) {
			result->push_back(kw);
		}
		else if (kw->object->object == kScriptObjectSemicolon) {
			result->push_back(kw);
		}
		else if (kw->object->object == kScriptObjectComa) {
			continue;
		}
		else if (kw->object->IsCondition()) {
			kw->Evaluate(this);
		}
		else {
			arguments = kw->object->arguments;

			if (arguments == -1) {
				continue;
			}

			if (result->size() < arguments) {
				PostSyntaxError(kw->object->line, "Invalid parameters, (%s) needs (%d) arguments", kw->object->word.data(), arguments);
			}

			if (kw->object->arguments == 1) {
				left = result->back();
				result->pop_back();
				this->EvaluateSingleExpression(left, kw);
			}
			else if (kw->object->arguments == 2) {
				right = result->back();
				result->pop_back();
				left = result->back();
				result->pop_back();
				this->EvaluateDoubleExpression(left, right, kw);
			}

			if (arguments) {
				result->push_back(left);
			}
		}
	}

	for (ScriptNodePtr node : *result) {
		if (node->object->object == kScriptObjectVariable) {
			if (!node->var) {
				ScriptNodePtr right_ = this->FindVar(node->object->word.data());
				if (!right_) {
					PostSyntaxError(node->object->line, "Undeclared variable (%s)", node->object->word.data());
				}
				node->object = right_->object;
			}
			if (node->object->type == kScriptTypeDefault) {
				StringC ptr = node->object->type.name.data();
				node->object->type.Parse(&ptr);
			}
		}
	}
}

Void ScriptPerformer::Trace(Void) {

	puts("------------------------");
	for (ScriptNodePtr node : this->typeList) {
		if (node->type == kScriptNodeFunction) {
			printf("%s %s.%s\n", node->object->word.data(), node->parent->object->type.name.data(), node->object->type.name.data());
		}
		else {
			printf("%s %s\n", node->object->word.data(), node->object->type.name.data());
		}
	}

	if (this->typeList.size()) {
		puts("------------------------");
	}

	for (ScriptNodePtr node : this->varList) {
		printf("%s %s = ", node->object->type.GetString(), node->object->word.data());
		switch (node->object->type) {
		case kScriptTypeBool:
			printf("%s", node->var->boolValue ? "TRUE" : "FALSE");
			break;
		case kScriptTypeFloat:
			printf("%.2f", node->var->floatValue);
			break;
		case kScriptTypeFunction:
			printf("FUNCTION");
			break;
		case kScriptTypeInt:
			printf("%lld", node->var->intValue);
			break;
		case kScriptTypeString:
			printf("\"%s\"", node->var->stringValue.data());
			break;
		default:
			break;
		}
		puts("");
	}

	if (this->varList.size()) {
		puts("------------------------");
	}
}

LAME_END
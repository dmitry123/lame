#include "Script.h"

LAME_BEGIN

#define __EvalError(_operation) \
	PostSyntaxError(left.object->line, "Unable to apply %s operation to this type", #_operation, left.type.GetString());

Void ScriptPerformer::Set(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
		__EvalError(/);
	}
}

Void ScriptPerformer::Mod(ScriptVariable& left, const ScriptVariable& right) {
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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
	switch (left.type) {
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

ScriptPerformer& ScriptPerformer::Evaluate(Void) {

	for (ScriptNodePtr node : this->nodeTree) {
		node->Evaluate(this);
	}
	return *this;
}

ScriptPerformer& ScriptPerformer::Trace(Void) {

	puts("------------------------");
	for (Map<Buffer, ScriptType>::iterator
			i = this->typeManager.typeMap.begin();
			i != this->typeManager.typeMap.end();
			i++
	) {
		printf("%s %s", i->first.data(), i->second.GetString());
	}

	if (this->typeManager.typeMap.size()) {
		puts("\n------------------------");
	}

	for (Map<Buffer, ScriptObjectPtr>::iterator
			i = this->varManager.varMap.begin();
			i != this->varManager.varMap.end();
			i++
	) {
		printf("%s %s = ", i->second->var->type.GetString(), i->first.data());
		switch (i->second->var->type) {
			case kScriptTypeBool:
				printf("%s", i->second->var->boolValue ? "TRUE" : "FALSE");
				break;
			case kScriptTypeFloat:
				printf("%.2f", i->second->var->floatValue);
				break;
			case kScriptTypeFunction:
				printf("FUNCTION");
				break;
			case kScriptTypeInt:
				printf("%lld", i->second->var->intValue);
				break;
			case kScriptTypeString:
				printf("\"%s\"", i->second->var->stringValue.data());
				break;
			default:
				break;
		}
		puts("");
	}

	return *this;
}

Void ScriptPerformer::EvaluateSingleExpression(
	ScriptNodePtr left,
	ScriptNodePtr token
) {
	if (!left->object->var && token->object->flag != kScriptType) {
		ScriptObjectPtr left_ = this->varManager.Find(left->object->word.data());
		if (!this->varManager.Find(left->object->word.data())) {
			PostSyntaxError(left->object->line, "Undeclared variable (%s)", left->object->word.data());
		}
		if (token->object->IsRightAssociated()) {
			this->tempList.push_back(*left_->var);
			left->object->var = &this->tempList.back();
		}
		else {
			left->object = left_;
		}
	}
	switch (token->object->flag) {
        case kScriptType:
            if (!this->varManager.Declare(left->object)) {
                PostSyntaxError(left->object->line, "Variable redeclaration (%s)", left->object->word.data());
            }
            left->object->var->type = *token->object->type;
            break;
        case kScriptIncrement:
            this->Inc(*left->object->var);
            break;
        case kScriptDecrement:
            this->Dec(*left->object->var);
            break;
        case kScriptNot:
            this->Not(*left->object->var);
            break;
        case kScriptBitNot:
            this->BitNot(*left->object->var);
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
        if (left->object->flag == kScriptVariable) {
            left->object->var = LAME_NULL;
        }
    }
    
	if (!left->object->var) {
		ScriptObjectPtr left_ = this->varManager.Find(left->object->word.data());
		if (!left_) {
			PostSyntaxError(left->object->line, "Undeclared variable (%s)", left->object->word.data());
		}
		if (token->object->IsRightAssociated()) {
			this->tempList.push_back(*left_->var);
			left->object->var = &this->tempList.back();
		}
		else {
			left->object = left_;
		}
		isUseVar = LAME_TRUE;
	}
	if (!right->object->var) {
		ScriptObjectPtr right_ = this->varManager.Find(right->object->word.data());
		if (!right_) {
			PostSyntaxError(right->object->line, "Undeclared variable (%s)", right->object->word.data());
		}
		this->tempList.push_back(*right_->var);
		right->object->var = &this->tempList.back();
	}

	if (!left->object->var->object) {
		left->object->var->object = left->object;
	}
	if (!right->object->var->object) {
		right->object->var->object = right->object;
	}

	if (!left->object->type) {
		left->object->type = &left->object->var->type;
	}
	if (!right->object->type) {
		right->object->type = &right->object->var->type;
	}

	switch (token->object->flag) {
	case kScriptAnd:
		this->And(*left->object->var, *right->object->var);
		break;
	case kScriptOr:
		this->Or(*left->object->var, *right->object->var);
		break;
	case kScriptAbove:
		this->Above(*left->object->var, *right->object->var);
		break;
	case kScriptAboveEqual:
		this->AboveEqual(*left->object->var, *right->object->var);
		break;
	case kScriptBellow:
		this->Bellow(*left->object->var, *right->object->var);
		break;
	case kScriptBellowEqual:
		this->BellowEqual(*left->object->var, *right->object->var);
		break;
	case kScriptEqual:
		this->Equal(*left->object->var, *right->object->var);
		break;
	case kScriptNotEqual:
		this->NotEqual(*left->object->var, *right->object->var);
		break;
	default:
		goto __EvaluateMathExpression;
	}
	left->object->var->type = kScriptTypeBool;
	isUseVar = LAME_FALSE;

	goto __Return;
__EvaluateMathExpression:

	ScriptVariable::Convert(*right->object->var, *left->object->var);

	switch (token->object->flag) {
	case kScriptSet:
		this->Set(*left->object->var, *right->object->var);
		break;
	case kScriptMul:
	case kScriptMulSet:
		this->Mul(*left->object->var, *right->object->var);
		break;
	case kScriptDiv:
	case kScriptDivSet:
		this->Div(*left->object->var, *right->object->var);
		break;
	case kScriptMod:
	case kScriptModSet:
		this->Mod(*left->object->var, *right->object->var);
		break;
	case kScriptAdd:
	case kScriptAddSet:
		this->Add(*left->object->var, *right->object->var);
		break;
	case kScriptSub:
	case kScriptSubSet:
		this->Sub(*left->object->var, *right->object->var);
		break;
	case kScriptBitShiftL:
	case kScriptBitShiftSetL:
		this->BitShiftL(*left->object->var, *right->object->var);
		break;
	case kScriptBitShiftR:
	case kScriptBitShiftSetR:
		this->BitShiftR(*left->object->var, *right->object->var);
		break;
	case kScriptBitAnd:
	case kScriptBitAndSet:
		this->BitAnd(*left->object->var, *right->object->var);
		break;
	case kScriptBitXor:
	case kScriptBitXorSet:
		this->BitXor(*left->object->var, *right->object->var);
		break;
	case kScriptBitOr:
	case kScriptBitOrSet:
		this->BitOr(*left->object->var, *right->object->var);
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

		if (kw->object->flag == kScriptInt ||
			kw->object->flag == kScriptFloat ||
			kw->object->flag == kScriptString ||
			kw->object->flag == kScriptDefault ||
			kw->object->flag == kScriptVariable
		) {
			result->push_back(kw);
		}
		else if (kw->object->flag == kScriptSemicolon) {
			result->push_back(kw);
		}
		else if (kw->object->flag == kScriptComa) {
			continue;
		}
		else if (kw->object->IsCondition()) {
			kw->Evaluate(this);
		}
		else {
			arguments = kw->object->args;

			if (arguments == -1) {
				continue;
			}

			if (result->size() < arguments) {
				PostSyntaxError(kw->object->line, "Invalid parameters, (%s) needs (%d) arguments", kw->object->word.data(), arguments);
			}

			if (kw->object->IsUnary()) {
				left = result->back();
				result->pop_back();
				this->EvaluateSingleExpression(left, kw);
			}
			else if (kw->object->IsBinary()) {
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
		if (node->object->flag == kScriptVariable) {
			if (!node->object->var) {
				ScriptObjectPtr right_ = this->varManager.Find(node->object->word.data());
				if (!right_) {
					PostSyntaxError(node->object->line, "Undeclared variable (%s)", node->object->word.data());
				}
				node->object = right_;
			}
			if (node->object->var->type == kScriptTypeDefault) {
				StringC ptr = node->object->var->type.word.data();
				node->object->var->type.Parse(&ptr);
			}
		}
	}
}

LAME_END
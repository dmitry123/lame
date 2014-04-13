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

Void ScriptPerformer::RegisterType(ScriptNodePtr node) {
	if (!this->vtManager_.DeclareType(&node->object->type)) {
		PostSyntaxError(node->object->line, "Type redeclaration (%s)", node->object->type.GetString());
	}
	node->object->object = kScriptObjectType;
}

Void ScriptPerformer::RegisterVar(ScriptNodePtr node) {
	if (!this->vtManager_.DeclareVar(node->var)) {
		PostSyntaxError(node->object->line, "Variable redeclaration (%s)", node->object->type.GetString());
	}
	node->var->declared = LAME_TRUE;
	node->object->object = kScriptObjectVariable;
}

ScriptTypePtr ScriptPerformer::FindType(const Buffer& name) {

	ScriptTypePtr type = this->vtManager_.FindType(name);

	if (!type) {
		PostSyntaxError(0, "Undeclared type (%s)", name);
	}

	return type;
}

ScriptVariablePtr ScriptPerformer::FindVar(const Buffer& name) {

	ScriptVariablePtr var = this->vtManager_.FindVar(name);

	if (!var) {
		PostSyntaxError(0, "Undeclared variable (%s)", name.data());
	}

	return var;
}

Void ScriptPerformer::GetVariableWithNode(ScriptNodePtr node, Bool right) {
    
	if (!node->var) {
        
		ScriptVariablePtr left_ = FindVar(node->object->word.data());
        
		if (!this->FindVar(node->object->word.data())) {
			PostSyntaxError(node->object->line, "Undeclared variable (%s)", node->object->word.data());
		}
        
		if (right) {
			this->tempList_.push_back({*left_, *left_->object});
            node->var = &this->tempList_.back().var;
            node->object = &this->tempList_.back().object;
            node->var->object = node->object;
		}
		else {
			node->object = left_->object;
			node->var = left_;
		}
	}
}

Void ScriptPerformer::RegisterConstant(ScriptNodePtr node) {

#pragma message LAME_TODO("add TRUE/FALSE for Bool type")

	if (node->object->object == kScriptObjectInt) {
		node->var->intValue = ScriptObject::ParseIntValue(node->object->word);
		*node->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectInt);
		node->object->type = kScriptTypeInt;
	}
	else if (node->object->object == kScriptObjectFloat) {
		node->var->floatValue = ScriptObject::ParseFloatValue(node->object->word);
		*node->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectFloat);
		node->object->type = kScriptTypeFloat;
	}
	else if (node->object->object == kScriptObjectBool) {
		node->var->boolValue = (ScriptNativeBool)ScriptObject::ParseIntValue(node->object->word);
		*node->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectBool);
		node->object->type = kScriptTypeBool;
	}
	else if (node->object->object == kScriptObjectString) {
		node->var->stringValue = ScriptObject::ParseStringValue(node->object->word);
		*node->object = *ScriptObject::FindScriptObjectByFlag(kScriptObjectString);
		node->object->type = kScriptTypeString;
	}
}

Void ScriptPerformer::EvaluateSingleExpression(
	ScriptNodePtr left,
	ScriptObjectPtr token
) {
    if (token->object != kScriptObjectType) {
        GetVariableWithNode(left, token->IsRightAssociated());
    }
    
	switch (token->object) {
        case kScriptObjectType:
            if (left->object->IsLeftAssociated()) {
				this->RegisterVar(left);
				left->object->type = token->type;
            }
            else {
                GetVariableWithNode(left, LAME_TRUE);
				left->var->Convert(token->type);
				left->object->type = token->type;
                left->var->object->type = left->object->type;
            }
            if (left->object->type == kScriptTypeVar) {
                left->var->modificators |= kScriptModificatorVar;
            }
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
	ScriptObjectPtr token
) {
	GetVariableWithNode(left, token->IsRightAssociated());
    GetVariableWithNode(right, LAME_TRUE);

	switch (token->object) {
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
    left->var->object->type = kScriptTypeBool;

	goto __Return;
__EvaluateMathExpression:
    
    right->var->Convert(left->var->object->type);
    
	switch (token->object) {
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

	this->tempList_.clear();

	for (Iterator i = list->begin(); i != list->end(); i++) {
		kw = *i;

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

		if (!kw->var->IsDeclared() && kw->object->IsVariable()) {
			this->RegisterVar(kw);
			this->Evaluate(&kw->block, result);
			result->clear();
		}
	}
}

Void ScriptPerformer::Trace(Void) {

	puts("\n----------");
	printf("- TYPES -\n");
	puts("----------\n");
	for (ScriptTypeManager::TypeMap& vm : this->vtManager_.GetTypeManager()->spaceMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			if (*i->first.data() != '$') {
				printf("%s\n", i->first.data());
			}
		}
	}
	puts("");

	puts("-------------");
	printf("- VARIABLES -\n");
	puts("-------------");
	for (ScriptVarManager::VarMap& vm : this->vtManager_.GetVarManager()->spaceMapQueue) {
		for (auto i = vm.begin(); i != vm.end(); i++) {
			printf("%s %s = ", i->second->object->type.GetString(), i->second->object->word.data());
			switch (i->second->object->type) {
				case kScriptTypeBool:
					printf("%s", i->second->boolValue ? "TRUE" : "FALSE");
					break;
				case kScriptTypeFloat:
					printf("%.2f", i->second->floatValue);
					break;
				case kScriptTypeFunction:
					printf("FUNCTION");
					break;
				case kScriptTypeInt:
					printf("%lld", i->second->intValue);
					break;
				case kScriptTypeString:
					printf("\"%s\"", i->second->stringValue.data());
					break;
				default:
					break;
			}
			puts("");
		}
	}
	puts("");
}

LAME_END
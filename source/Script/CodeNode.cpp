#include "CodeNode.h"

LAME_BEGIN2(Script)

Buffer CodeNode::GetCodeName(Code code) {

	switch (code) {
	case Code::Noop:
		return "noop";
	case Code::Cast:
		return "cast";
	case Code::Increment:
		return "inc";
	case Code::Decrement:
		return "dec";
	case Code::InstanceOf:
		return "instanceof";
	case Code::Return:
		return "return";
	case Code::BitNot:
		return "bitnot";
	case Code::Not:
		return "not";
	case Code::Minus:
		return "minus";
	case Code::Plus:
		return "plus";
	case Code::New:
		return "new";
	case Code::Throw:
		return "throw";
	case Code::Mul:
		return "mul";
	case Code::Div:
		return "div";
	case Code::Mod:
		return "mod";
	case Code::Add:
		return "add";
	case Code::Sub:
		return "sub";
	case Code::ShiftL:
		return "shiftl";
	case Code::ShiftR:
		return "shiftr";
	case Code::ShiftU:
		return "shiftu";
	case Code::And:
		return "and";
	case Code::Xor:
		return "xor";
	case Code::Or:
		return "or";
	case Code::Assign:
		return "assign";
	case Code::Bellow:
		return "bellow";
	case Code::BellowEqual:
		return "bellowe";
	case Code::Above:
		return "above";
	case Code::AboveEqual:
		return "abovee";
	case Code::Equal:
		return "equal";
	case Code::NotEqual:
		return "notequal";
	case Code::If:
		return "if";
	case Code::Invoke:
		return "invoke";
	case Code::Load:
		return "load";
	case Code::Skip:
		return "skip";
	}

	return "noop";
}

LAME_END2
#ifndef __LAME_SCRIPT_CODE_NODE__
#define __LAME_SCRIPT_CODE_NODE__

#include "Node.h"

LAME_BEGIN2(Script)

class LAME_API CodeNode {
public:
	enum class Code : Uint32 {
		Noop,
		Cast,
		Increment,
		Decrement,
		InstanceOf,
		Return,
		BitNot,
		Not,
		Minus,
		Plus,
		New,
		Throw,
		Mul,
		Div,
		Mod,
		Add,
		Sub,
		ShiftL,
		ShiftR,
		ShiftU,
		And,
		Xor,
		Or,
		Assign,
		Bellow,
		BellowEqual,
		Above,
		AboveEqual,
		Equal,
		NotEqual,
		If,
		Invoke,
		Load,
		Store,
		Skip,
		Clone
	};
public:
	CodeNode(Code code, NodePtr node, ObjectPtr left, ObjectPtr right) :
		code(code), node(node), left(left), right(right), line(node->lex->line), offset(-1)
	{
	}
public:
	static Buffer GetCodeName(Code code);
public:
	inline Buffer GetName() {
		return this->GetCodeName(this->code);
	}
	inline Void SetOffset(Uint32 offset) {
		this->offset = offset;
	}
public:
	inline Code      GetCode()   { return this->code;     }
	inline ObjectPtr GetLeft()   { return this->left;     }
	inline ObjectPtr GetRight()  { return this->right;    }
	inline NodePtr   GetNode()   { return this->node;     }
	inline Uint32    GetLine()   { return this->line;     }
	inline Uint32    GetOffset() { return this->offset;   }
public:
	inline Vector<CodeNodePtr>& GetList() {
		return this->codeList;
	}
private:
	const Code      code;
	const ObjectPtr left;
	const ObjectPtr right;
	const NodePtr   node;
	const Uint32    line;
private:
	Vector<CodeNodePtr> codeList;
	Uint32 offset;
};

class LAME_API CodeMethod {
public:
	CodeMethod(MethodPtr method) :
		method(method)
	{
	}
public:
	inline MethodPtr GetMethod() {
		return this->method;
	}
	inline Vector<CodeNodePtr>& GetList() {
		return this->codeList;
	}
public:
	inline Void Insert(CodeNodePtr codeNode) {
		this->codeList.push_back(codeNode);
	}
private:
	MethodPtr method;
	Vector<CodeNodePtr> codeList;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CODE_NODE__
#ifndef __LAME_SCRIPT_CODE_NODE__
#define __LAME_SCRIPT_CODE_NODE__

#include "Method.h"

LAME_BEGIN2(Script)

class LAME_API CodeNode {
public:
	CodeNode(MethodPtr method) :
		method(method), node(method->GetRootNode())
	{
		this->_Defaults();
	}
public:
	inline Buffer GetName() {
		return this->method->GetPath() + this->method->GetName();
	}
public:
	inline MethodPtr GetMethod() { return this->method; }
	inline NodePtr   GetNode()   { return this->node;   }
	inline Uint32    GetLine()   { return this->line;   }
	inline Uint32    GetArgs()   { return this->args;   }
	inline Uint32    GetLocals() { return this->locals; }
public:
	inline Deque<NodePtr>& GetList() {
		return this->method->GetRootNode()->blockList;
	}
private:
	Void _Defaults();
private:
	const MethodPtr method;
	const NodePtr   node;
private:
	Uint32 line;
	Uint32 args;
	Uint32 locals;
};

LAME_END2

#endif // ~__LAME_SCRIPT_CODE_NODE__
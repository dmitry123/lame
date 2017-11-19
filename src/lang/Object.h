#ifndef __LAME_LANG__OBJECT__
#define __LAME_LANG__OBJECT__

#include "Foundation.h"
#include "Node.h"
#include "MurmurHash.h"

LAME_LANG_BEGIN

typedef class Object {
	typedef Object* ObjectPtr;
public:
	typedef enum {
		MODIFIER_INVALID = 0x0000,
		MODIFIER_PUBLIC = 0x0001,
		MODIFIER_PROTECTED = 0x0002,
		MODIFIER_PRIVATE = 0x0004,
		MODIFIER_FINAL = 0x0008,
		MODIFIER_NATIVE = 0x0010,
		MODIFIER_STATIC = 0x0020
	} Modifier;
public:
	typedef enum {
		TYPE_INVALID,
		TYPE_BYTE,
		TYPE_SHORT,
		TYPE_INTEGER,
		TYPE_LONG,
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_BOOLEAN,
		TYPE_CHAR,
		TYPE_CLASS,
		TYPE_METHOD
	} Type;
public:
	typedef UInt64 Hash;
public:
	Object(NodePtr node, ObjectPtr parent = nullptr) :
		parent(parent),
		node(node),
		attached(nullptr),
		cache(nullptr),
		modifiers(0),
		type(TYPE_INVALID)
	{
		if (parent != nullptr) {
			parent->insert(this);
		}
		else {
			this->hash = this->calculateHash();
		}
	}
	~Object();
public:
	void insert(ObjectPtr object);
	Logic erase(ObjectPtr object);
	ObjectPtr find(Hash hash, Logic depth = true);
	ObjectPtr find(StringPtrC name, Logic depth = true);
	void merge(ObjectPtr object);
	ObjectPtr clone(ObjectPtr parent);
	void move(ObjectPtr object);
	UInt32 count();
	void clear();
	StringPtrC path();
	ObjectPtr root();
public:
	inline Logic erase() {
		return this->parent != nullptr && this->parent->erase(this);
	}
public:
	inline Logic isAnnotation() {
		return this->type == TYPE_CLASS && this->getNode() && this->getNode()->is(Node::FLAG_ANNOTATION);
	}
	inline Logic isClass() {
		return this->type == TYPE_CLASS && this->getNode() && this->getNode()->is(Node::FLAG_CLASS);
	}
	inline Logic isInterface() {
		return this->type == TYPE_CLASS && this->getNode() && this->getNode()->is(Node::FLAG_INTERFACE);
	}
	inline Logic isMethod() {
		return this->type == TYPE_METHOD && !(this->modifiers & MODIFIER_STATIC);
	}
	inline Logic isFunction() {
		return this->type == TYPE_METHOD && (this->modifiers & MODIFIER_STATIC) != 0;
	}
	inline Logic isConstruction() {
		return this->getNode() && this->getNode()->isConstruction();
	}
public:
	inline Logic isPublic() {
		return (this->modifiers & MODIFIER_PUBLIC) != 0;
	}
	inline Logic isProtected() {
		return (this->modifiers & MODIFIER_PROTECTED) != 0;
	}
	inline Logic isPrivate() {
		return (this->modifiers & MODIFIER_PRIVATE) != 0;
	}
	inline Logic isFinal() {
		return (this->modifiers & MODIFIER_FINAL) != 0;
	}
	inline Logic isNative() {
		return (this->modifiers & MODIFIER_NATIVE) != 0;
	}
	inline Logic isStatic() {
		return (this->modifiers & MODIFIER_STATIC) != 0;
	}
public:
	inline ObjectPtr setModifiers(UInt32 modifiers) {
		this->modifiers = modifiers;
		return this;
	}
	inline ObjectPtr setType(Type type) {
		this->type = type;
		return this;
	}
	inline ObjectPtr setAttached(ObjectPtr attached) {
		this->attached = attached;
		return this;
	}
public:
	inline NodePtr getNode() {
		return this->node;
	}
	inline ObjectPtr getParent() {
		return this->parent;
	}
	inline Map<Hash, ObjectPtr>& getChildren() {
		return this->children;
	}
	inline Hash getHash() {
		return this->hash;
	}
	inline UInt32 getModifiers() {
		return this->modifiers;
	}
	inline Type getType() {
		return this->type;
	}
	inline ObjectPtr getAttached() {
		return this->attached;
	}
protected:
	Hash calculateHash();
private:
	NodePtr node;
	Map<Hash, ObjectPtr> children;
	StringPtrC cache;
	Hash hash;
	ObjectPtr parent;
	UInt32 modifiers;
	Type type;
	ObjectPtr attached;
} *ObjectPtr;

LAME_LANG_END

#endif // ~__LAME_LANG__OBJECT__
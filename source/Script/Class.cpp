#include "Class.h"
#include "Lex.h"
#include "Exception.h"
#include "Variable.h"
#include "Method.h"
#include "Node.h"
#include "Interface.h"

LAME_BEGIN2(Script)

Class::Class(BufferRefC name, ScopePtr parent, Uint32 size) :
    Class(name, parent, Type::Class, size)
{
}

Class::~Class() {
    // ignore
}

Bool Class::Equal(ObjectPtrC object) {
	return this->Hash() == object->Hash();
}

ObjectPtr Class::Clone(BufferRefC name, ObjectPtr parent) {

	ClassPtr newClass =
		new Class(name, parent);

	newClass->Scope::Clone(this);

	return newClass;
}

Void Class::Trace(Uint32 offset) {

	printf(this->GetModificatorString().data());
    
    StringC languageType = this->CheckType(Type::Class) ? "class" : "interface";
    
	if (this->GetTemplate()) {
		printf("%s %s <%s> ", languageType, this->GetName().data(), this->GetTemplate()->GetName().data());
	}
	else {
		if (!this->GetName().length()) {
			printf("%s (%d) ", languageType, this->GetNode()->lex->line);
		}
		else {
			printf("%s %s ", languageType, this->GetName().data());
		}
	}

	if (this->extendClass) {
		printf("extends %s%s ",
			this->extendClass->GetPath().data(),
			this->extendClass->GetName().data());
	}

	if (this->implementClass.size() > 0) {

		printf("implements ");

		for (ObjectPtr c : this->implementClass) {
			printf("%s, ", c->GetName().data());
		}
	}

	printf("{");

	if (this->Amount() > 0) {
		this->Scope::Trace(offset + 1);
		Lex::PrintLine(offset);
	}

	printf("}");
}

Class::HashType Class::Hash(Void) {
	return this->GetHash64();
}

Uint32 Class::Size(Void) {

	if (this->CheckModificator(Modificator::Primitive)) {
		return this->sizeOf;
	}

	return this->sizeOf;

	//return this->Scope::Size();
}

ClassPtr Class::GetClass(Void) {
	return this;
}

Void Class::New(ObjectPtr object) {

	// check object for variable, cuz we can
	// store class's objects only in variables
	if (!object->CheckType(Type::Variable)) {
		PostSyntaxError(this->GetNode()->lex->line, "Unable to apply operator new for non-variable object", 0);
	}

	// get object's variable, its true (checking upper)
	VariablePtr var = object->GetVariable();
	var->SetObject(new Class(this->GetName(), object->GetParent()));
	var->objectValue->Scope::Clone(this);

	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
	if (this->extendClass != NULL && this->extendClass->CheckModificator(Modificator::Abstract)) {
		for (ObjectPtr i : this->extendClass->GetMethodSet()) {
            var->objectValue->Find(i->Hash())->GetMethod()->SetThis(var);
		}
	}

	// if we have implemented interfaces, then
	// we must do the same thing that with
	// abstract class
	for (ObjectPtr i : this->implementClass) {
		for (ObjectPtr i : i->GetMethodSet()) {
			var->objectValue->Find(i->Hash())->GetMethod()->SetThis(var);
		}
	}

	// activate all class'es methods
	for (ObjectPtr i : var->GetObject()->GetMethodSet()) {
		MethodPtr mThis = var->objectValue->Find(i->Hash())->GetMethod();
		mThis->SetThis(var);
	}
}

Void Class::Extend(ObjectPtr object) {

	if (this->GetType() == Type::Interface) {
		PostSyntaxError(object->GetNode()->lex->line, "Interface can't extend classes (%s)", object->GetName().data());
	}

	if (this->extendClass != NULL) {
		PostSyntaxError(this->GetNode()->lex->line, "Class has already been extended (%s)", object->GetName().data());
	}

	this->extendClass = object;

	if (object->GetType() == Object::Type::Interface) {
		for (ObjectPtr i : object->GetInterface()->implementClass) {
			this->Implement(i);
		}
	}
	else if (object->GetType() == Object::Type::Class) {
		for (ObjectPtr i : object->GetClass()->implementClass) {
			this->Implement(i);
		}
	}
	else{
		// :(
	}
}

Void Class::Implement(ObjectPtr object) {

	if (this->GetType() == Object::Type::Interface) {

		if (this->implementClass.count(object)) {
			PostSyntaxError(this->GetNode()->lex->line, "Interface has already implement that interface (%s)",
				object->GetName().data());
		}
        
		if (!object->CheckType(Type::Interface)) {
			PostSyntaxError(this->GetNode()->lex->line, "Unable to implement non-interface object (%s)",
				object->GetName().data());
		}

		this->implementClass.insert(object);
	}
	else {

		if (this->implementClass.count(object)) {
			PostSyntaxError(this->GetNode()->lex->line, "Class has already been implemented with that class (%s)",
				object->GetName().data());
		}

		if (!object || !object->CheckType(Type::Interface)) {
			PostSyntaxError(this->GetNode()->lex->line, "Class can only implement interfaces (%s)",
				object->GetName().data());
		}

		this->implementClass.insert(object);
	}
}

Void Class::CheckInheritance(Void) {
    
    for (ObjectPtr object : this->implementClass) {
        
		// look thought all methods
		for (auto i : object->GetMethodSet()) {
            
			MethodPtr m = i->GetMethod();
			MethodPtr m2 = NULL;
            
			// if method has root node then
			// throw an error, cuz interface's
			// methods mustn't be implemented
			if (m->GetRootNode() != NULL) {
                PostSyntaxError(this->GetNode()->lex->line, "Interface mustn't implement methods (%s)", m->GetName().data());
			}
            
            Bool isFound = FALSE;
            
			// now lets try to find overloaded methods
			// in current class (this), if we cant, then
			// throw an error, cuz class, which implements
			// interface have to implement it's methods
            for (auto j : this->GetMethodSet()) {
                if (j->GetName() == i->GetName()) {
                    if (j->GetMethod()->GetInvokeHash() ==
                        i->GetMethod()->GetInvokeHash()
                    ) {
                        if (j->GetMethod()->GetRootNode()) {
							m2 = j->GetMethod(); isFound = TRUE; break;
                        }
                    }
                }
            }
            
            if (!isFound || m->GetReturnType() != m2->GetReturnType()) {
				PostSyntaxError(this->GetNode()->lex->line, "Class (%s) must implement interface method \"%s %s(%s)\"",
					this->GetName().data(), i->GetMethod()->GetReturnType()->GetName().data(), m->GetName().data(),
					m->GetFormattedArguments().data());
            }
		}
        
        this->Merge(object);
    }
    
	// now we have to check extended class for
	// abstract, if it true, then we must
	// change method's reference and it's this
	// object
    if (this->extendClass) {
        
        if (this->extendClass->CheckModificator(Modificator::Abstract)) {
            
            // walk though all methods
            for (ObjectPtr i : this->extendClass->GetMethodSet()) {
                
                MethodPtr m = i->GetMethod();
                
                if (!m->CheckModificator(Modificator::Abstract)) {
                    continue;
                }
                
                // if method has root node then
                // throw an error, cuz interface's
                // methods mustn't be implemented
                if (m->GetRootNode() != NULL) {
                    PostSyntaxError(this->GetNode()->lex->line, "Abstract class mustn't implement abstract methods (%s)", m->GetName().data());
                }
                
                Bool isFound = FALSE;
                
                // now lets try to find overloaded methods
                // in current class (this), if we cant, then
                // throw an error, cuz class, which implements
                // interface have to implement it's methods
                for (auto j : this->GetMethodSet()) {
                    if (j->GetName() == i->GetName()) {
                        if (j->GetMethod()->GetInvokeHash() ==
                            i->GetMethod()->GetInvokeHash()
                        ) {
                            if (j->GetMethod()->GetRootNode()) {
                                isFound = TRUE; break;
                            }
                        }
                    }
                }
                
                if (!isFound) {
                    PostSyntaxError(this->GetNode()->lex->line, "Class (%s) must implement abstract method (%s)", this->GetName().data(), m->GetName().data());
                }
            }
        }
        
        this->Merge(this->extendClass);
    }
}

Class::Class(BufferRefC name, ScopePtr parent, Type type, Uint32 size) : Object(name, parent, type),
	extendClass(0),
	priority(0)
{
	this->sizeOf = size;

	if (!this->sizeOf) {
		this->sizeOf = SizeOf;
	}

    this->SetOnScopeUpdate([] (ScopePtr scope, ObjectPtr object) {

        ClassPtr self = ClassPtr(scope);

		if (object->CheckType(Object::Type::Method)) {
			if (!object->CheckModificator(Object::Modificator::Native) && object->GetNode() && !(object->GetNode()->flags & kScriptFlagImplemented)) {
				PostSyntaxError(object->GetNode()->lex->line, "Only native methods don't have to be implemented %s(%s)",
					object->GetName().data(), object->GetMethod()->GetFormattedArguments().data());
			}
		}
        
        if (self->CheckType(Object::Type::Interface)) {
            if (object->CheckType(Object::Type::Method) && object->GetMethod()->GetRootNode()) {
                PostSyntaxError(object->GetNode()->lex->line,
                    "Interface can't implement methods %s(%s)", object->GetName().data(),
					object->GetMethod()->GetFormattedArguments().data());
            }
        }
        else {
            if (object->CheckType(Object::Type::Method) && object->GetMethod()->GetNode()) {
                if ((object->GetMethod()->GetNode()->flags & kScriptFlagAbstract) != 0 &&
                        object->GetMethod()->GetRootNode()
                ) {
                    PostSyntaxError(object->GetMethod()->GetRootNode()->lex->line,
                        "Abstract class can't implement abstract methods %s(%s)", object->GetName().data(),
						object->GetMethod()->GetFormattedArguments().data());
                }
            }
        }
    });
}

LAME_END2

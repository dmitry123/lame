#ifndef __LAME_SCRIPT_LAME_COMPILER__
#define __LAME_SCRIPT_LAME_COMPILER__

#include "AbstractCompiler.h"

LAME_BEGIN2(Script)

class LAME_API LameCompiler : public AbstractCompiler::Inspector {
public:
	LameCompiler(AbstractCompilerPtr abstractCompiler) :
		Inspector(abstractCompiler)
	{
		this->GetCompiler()->Inspect(this);
	}
public:
	Void onBinary(ObjectPtr left, ObjectPtr right) override;
	Void onUnary(ObjectPtr object)                 override;
	Void onCondition(ObjectPtr object)             override;
	Void onReturn(ObjectPtr object)                override;
	Void onLoad(ObjectPtr object)                  override;
	Void onStore(ObjectPtr object)                 override;
	Void onInvoke(ObjectPtr method)                override;
	Void onCast(ObjectPtr object, ClassPtr type)   override;
	Void onClone(ObjectPtr object)                 override;
	Void onNew(ObjectPtr object, Uint32 size)      override;
};

LAME_END2

#endif // ~__LAME_SCRIPT_LAME_COMPILER__
#ifndef __LAME_WINDOW_SYSTEM__STATE_DELEGATE__
#define __LAME_WINDOW_SYSTEM__STATE_DELEGATE__

#include "Define.h"

LAME_BEGIN

class LAME_API StateDelegate {
public:
	enum class Flag {
		Attach = 0x0001,
		Detach = 0x0002,
		Render = 0x0004,
		Change = 0x0008,
		Load   = 0x0010,
		UnLoad = 0x0020,
		Set    = 0x0040
	};
public:
	virtual void onStateAttach(Void);
	virtual void onStateDetach(Void);
	virtual void onStateRender(Void);
	virtual void onStateChange(StatePtr state);
	virtual void onStateLoad(Void);
	virtual void onStateUnLoad(Void);
	virtual void onStateSet(StatePtr left, StatePtr right);
public:
	inline Void Enable(Flag flag) {
		this->flags |= (Uint32) flag;
	}
	inline Void Disable(Flag flag) {
		this->flags &= ~(Uint32)flag;
	}
	inline Bool Check(Flag flag) {
		return (this->flags & (Uint32)flag) != 0;
	}
private:
	Uint32 flags = -1;
};

LAME_END

#endif // ~__LAME_WINDOW_SYSTEM__STATE_DELEGATE__
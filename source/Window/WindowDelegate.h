#ifndef __LAME_WINDOW__WINDOW_DELEGATE__
#define __LAME_WINDOW__WINDOW_DELEGATE__

#include "Define.h"

LAME_BEGIN

class WindowDelegate {
public:
	virtual void onWindowAttach() {}
	virtual void onWindowDetach() {}
	virtual void onWindowRender() {}
	virtual void onWindowTick() {}
	virtual void onWindowReshape() {}
	virtual void onWindowMouseClick() {}
	virtual void onWindowMouseDown() {}
	virtual void onWindowMouseUp() {}
	virtual void onWindowMouseMove() {}
	virtual void onWindowMouseDrag() {}
	virtual void onWindowKeyDown() {}
	virtual void onWindowKeyUp() {}
	virtual void onWindowKeyType() {}
};

LAME_END

#endif // ~__LAME_WINDOW__WINDOW_DELEGATE__
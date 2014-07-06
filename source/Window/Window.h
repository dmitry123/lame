#ifndef __WINDOW_SYSTEM__WINDOW__
#define __WINDOW_SYSTEM__WINDOW__

#include "Define.h"
#include "Display.h"
#include "WindowDelegate.h"

LAME_BEGIN

class LAME_API Window : public WindowDelegate {
	friend class Display;
	friend class Display::_Display;
public:
	 Window(StringC name);
	~Window();
public:
	inline StringC GetName() {
		return this->name;
	}
	inline Display GetDisplay() {
		return this->dipslay;
	}
public:
	WindowPtr SetPosition(Sint32 x, Sint32 y);
	WindowPtr SetSize(Uint32 width, Uint32 height);
	WindowPtr SetTitle(StringC title);
public:
	WindowPtr Create(Sint32 argc, StringC argv[]);
private:
	Display dipslay;
	StringC name;
	Point position;
	Uint32 width;
	Uint32 height;
	StringC title;
	Uint32 id;
};

LAME_END

#endif // ~__WINDOW_SYSTEM__WINDOW__
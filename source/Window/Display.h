#ifndef __LAME_WINDOW_SYSTEM__DISPLAY__
#define __LAME_WINDOW_SYSTEM__DISPLAY__

#include "Define.h"

LAME_BEGIN

class LAME_API Display {
	friend class Window;
private:
	typedef class LAME_API _Display {
		friend class Window;
	public:
		_Display();
	public:
		inline Uint32 GetWidth() {
			return this->width;
		}
		inline Uint32 GetHeight() {
			return this->height;
		}
		inline WindowPtr GetCurrentWindow() {
			return this->current;
		}
		inline PointPtr GetMousePosition() {
			return &this->mousePosition;
		}
		inline Uint32 GetPressedKey() {
			return this->key;
		}
	public:
		template <class T, typename... Args> inline void New(Args... args) {
			this->Attach(new T(args...));
		}
	public:
		WindowPtr Find(StringC name);
		Bool Attach(WindowPtr window);
		Bool Detach(WindowPtr window);
		Bool Detach(StringC name);
		Bool Loop(Void);
	private:
		Void _RegisterWindow(WindowPtr window, StringC name);
		Void _UnRegisterWindow(WindowPtr window);
	private:
		Uint32 width;
		Uint32 height;
		Point mousePosition;
		Uint32 key;
	private:
		WindowPtr current;
	private:
		Map<StringC, WindowPtr> hash;
	} *_DisplayPtr;
public:
	inline _DisplayPtr operator -> () {
		return info;
	}
public:
	Display() {
	}
private:
	static _DisplayPtr info;
};

LAME_END

#endif // ~__LAME_WINDOW_SYSTEM__DISPLAY__
#ifndef __LAME_CORE__EVENT__
#define __LAME_CORE__EVENT__

#include "Define.h"
#include "List.h"

#include <functional>

LAME_BEGIN

template <class T> class Event : public std::function <T> {
public:
	Event() {
		// nothing
	}
public:
	template <class F> Event(F f) : std::function <T>(f) {
		// nothing
	}
};

template <class T> class EventQueue {
public:
	typedef Event <T> Proc;
public:
	Void InsertEvent(Proc callback) {
		this->queue_.push_back(callback);
	}
	Void Invoke(Void) {
		for (Proc p : this->list_) {
			p();
		}
	}
	Void Clear(Void) {
		this->queue_.clear();
	}
public:
	inline Uint32 GetSize() const {
		return this->queue_.size();
	}
private:
	List <Proc> queue_;
};

LAME_END

#endif // ~__LAME_CORE__EVENT__
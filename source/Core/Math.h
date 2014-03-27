#ifndef __LAME_CORE__MATH__
#define __LAME_CORE__MATH__

#include "Types.h"

LAME_BEGIN

class LAME_API ElasticBox {
public:
	typedef enum {
		kNeedNothing = 0x0000,
		kNeedMore = 0x0001,
		kNeedWidth = 0x0002,
		kNeedHeight = 0x0003
	} Needs;
public:
	Void AdjustChildren();
	Void Resize(Uint32 x, Uint32 y, Uint32 width, Uint32 height);
	Uint32 FindSpace(Uint32P x, Uint32P y, Uint32 width, Uint32 height);
	Void InsertItem(Uint32P x, Uint32P y, Uint32 width, Uint32 height);
public:
	inline Uint32 GetWidth() {
		return this->w;
	}
	inline Uint32 GetHeight() {
		return this->h;
	}
public:
	ElasticBox();
	~ElasticBox();
public:
	ElasticBox(Uint32 w, Uint32 h) : w(w), h(h) {
		// nothing
	}
private:
	Uint32 item_w;
	Uint32 item_h;
	Uint32 x;
	Uint32 y;
	Uint32 w;
	Uint32 h;
private:
	ElasticBox** children4;
};

Float32 LAME_API powf(Float32 x, Float32 y);
Float64 LAME_API powd(Float64 x, Float64 y);
Float32 LAME_API powl(Sint32 x, Sint32 y);
Float64 LAME_API powll(Sint64 x, Sint64 y);
Float32 LAME_API sqrtf(Float32 x);
Float64 LAME_API sqrtd(Float64 x);
Float32 LAME_API sqrtl(Sint32 x);
Float64 LAME_API sqrtll(Sint64 x);
Float32 LAME_API sqrf(Float32 x);
Float64 LAME_API sqrd(Float64 x);
Sint32 LAME_API sqrl(Sint32 x);
Sint64 LAME_API sqrll(Sint64 x);
Float32 LAME_API fabs(Float32 x);
Float64 LAME_API dabs(Float64 x);
Sint32 LAME_API labs(Sint32 x);
Sint64 LAME_API llabs(Sint64 x);
Float32 LAME_API pow(Float32 x, Float32 y);
Float64 LAME_API pow(Float64 x, Float64 y);
Float32 LAME_API pow(Sint32 x, Sint32 y);
Float64 LAME_API pow(Sint64 x, Sint64 y);
Float32 LAME_API sqrt(Float32 x);
Float64 LAME_API sqrt(Float64 x);
Float32 LAME_API sqrt(Sint32 x);
Float64 LAME_API sqrt(Sint64 x);
Float32 LAME_API sqr(Float32 x);
Float64 LAME_API sqr(Float64 x);
Sint32 LAME_API sqr(Sint32 x);
Sint64 LAME_API sqr(Sint64 x);
Float32 LAME_API abs(Float32 x);
Float64 LAME_API abs(Float64 x);
Sint32 LAME_API abs(Sint32 x);
Sint64 LAME_API abs(Sint64 x);

LAME_END

#endif // ~__LAME_CORE__MATH__
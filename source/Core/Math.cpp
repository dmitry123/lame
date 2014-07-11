#include "Math.h"

#if defined(LAME_MSVC) && !defined(_USE_MATH_DEFINES)
#  define _USE_MATH_DEFINES
#endif

#include <math.h>

LAME_BEGIN2(Core)

namespace internal {

	Float32 powf(Float32 x, Float32 y) {
		return ::powf(x, y);
	}

	Float64 powd(Float64 x, Float64 y) {
		return ::pow(x, y);
	}

	Float32 powl(Sint32 x, Sint32 y) {
		return ::pow((Float32)x, y);
	}

	Float64 powll(Sint64 x, Sint64 y) {
		return ::pow((Float64)x, (Float64)y);
	}

	Float32 sqrtf(Float32 x) {
		return ::sqrtf(x);
	}

	Float64 sqrtd(Float64 x) {
		return ::sqrt(x);
	}

	Float32 sqrtl(Sint32 x) {
		return ::sqrt((Float32)x);
	}

	Float64 sqrtll(Sint64 x) {
		return ::sqrt((Float64)x);
	}

	Float32 sqrf(Float32 x) {
		return x * x;
	}

	Float64 sqrd(Float64 x) {
		return x * x;
	}

	Sint32 sqrl(Sint32 x) {
		return x * x;
	}

	Sint64 sqrll(Sint64 x) {
		return x * x;
	}

	Float32 fabs(Float32 x) {
		return ::fabs(x);
	}

	Float64 dabs(Float64 x) {
		return ::fabs(x);
	}

	Sint32 labs(Sint32 x) {
		return (Sint32)::fabsl(x);
	}

	Sint64 llabs(Sint64 x) {
		return (Sint64) ::fabsl((Float96) x);
	}
}

Float32 Math::pow(Float32 x, Float32 y) {
	return internal::powf(x, y);
}

Float64 Math::pow(Float64 x, Float64 y) {
	return internal::powd(x, y);
}

Float32 Math::pow(Sint32 x, Sint32 y) {
	return internal::powl(x, y);
}

Float64 Math::pow(Sint64 x, Sint64 y) {
	return internal::powll(x, y);
}

Float32 Math::sqrt(Float32 x) {
	return internal::sqrtf(x);
}

Float64 Math::sqrt(Float64 x) {
	return internal::sqrtd(x);
}

Float32 Math::sqrt(Sint32 x) {
	return internal::sqrtl(x);
}

Float64 Math::sqrt(Sint64 x) {
	return internal::sqrtll(x);
}

Float32 Math::sqr(Float32 x) {
	return internal::sqrf(x);
}

Float64 Math::sqr(Float64 x) {
	return internal::sqrd(x);
}

Sint32 Math::sqr(Sint32 x) {
	return internal::sqrl(x);
}

Sint64 Math::sqr(Sint64 x) {
	return internal::sqrll(x);
}

Float32 Math::abs(Float32 x) {
	return internal::fabs(x);
}

Float64 Math::abs(Float64 x) {
	return internal::dabs(x);
}

Sint32 Math::abs(Sint32 x) {
	return internal::labs(x);
}

Sint64 Math::abs(Sint64 x) {
	return internal::llabs(x);
}

ElasticBox::ElasticBox() {

	this->children4 = 0;
	this->item_h = 0;
	this->item_w = 0;
	this->x = 0;
	this->y = 0;
	this->w = 100;
	this->h = 100;
}

ElasticBox::~ElasticBox() {

	if (this->children4) {
		for (Uint32 i = 0; i < 4; i++) {
			if (this->children4[i]) {
				delete this->children4[i];
			}
		}
		delete[] this->children4;
	}
}

Void ElasticBox::AdjustChildren() {

	if (this->children4[0]) {
		this->children4[0]->Resize(
			this->x + this->item_w, this->y, this->w - this->item_w, this->h - this->y);
	}
	if (this->children4[1]) {
		this->children4[1]->Resize(
			this->x, this->y + this->item_h, this->item_w, this->h - this->item_h);
	}
	if (this->children4[2]) {
		this->children4[2]->Resize(
			this->x, this->y + this->item_h, this->w - this->x, this->h - this->item_h);
	}
	if (this->children4[3]) {
		this->children4[3]->Resize(
			this->x + this->item_w, this->y, this->w - this->item_w, this->item_h);
	}
}

Void ElasticBox::Resize(Uint32 x, Uint32 y, Uint32 width, Uint32 height) {

	this->x = x;
	this->y = y;
	this->w = width;
	this->h = height;

	if (this->children4) {
		this->AdjustChildren();
	}
}

Uint32 ElasticBox::FindSpace(Uint32P x, Uint32P y, Uint32 item_w, Uint32 item_h) {

	Uint32 space_found = kNeedNothing;
	Uint32 i;

	if (item_w > w && item_h > h) {
		return kNeedMore;
	}
	if (item_w > w)	{
		return kNeedWidth;
	}
	if (item_h > h)	{
		return kNeedHeight;
	}

	if (!children4) {

		*x = this->x;
		*y = this->y;

		this->item_w = item_w;
		this->item_h = item_h;

		space_found = kNeedNothing;

		children4 = new ElasticBox*[4];

		for (i = 0; i < 4; i++) {
			children4[i] = new ElasticBox(w, h);
		}

		AdjustChildren();
	}
	else
	{
		for (i = 0; i < 4; i++) {
			if (this->children4[i] && (
				space_found = this->children4[i]->FindSpace(x, y, item_w, item_h)
			) == kNeedNothing) {
				break;
			}
		}

		if (space_found == kNeedNothing) {
			switch (i) {
			case 0:
			case 1:

				delete this->children4[2];
				delete this->children4[3];

				this->children4[2] = 0;
				this->children4[3] = 0;

				break;
			case 2:
			case 3:
				
				delete this->children4[0];
				delete this->children4[1];
				
				this->children4[0] = 0;
				this->children4[1] = 0;

				break;
			}
		}
	}

	return space_found;
}

Void ElasticBox::InsertItem(Uint32P x, Uint32P y, Uint32 width, Uint32 height) {

	Uint32 space_found;

	while ((space_found = FindSpace(x, y, width, height)) != kNeedNothing) {
		switch (space_found) {
		case kNeedWidth: {
			Resize(0, 0, this->w + 64, this->h);
		} break;
		case kNeedHeight: {
			Resize(0, 0, this->w, this->h + 64);
		} break;
		default: {
			Resize(0, 0, this->w + 64, this->h + 64);
		} break;
		}
	}
}

LAME_END2

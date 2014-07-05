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

LAME_END2

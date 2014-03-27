#include "Math.h"

#if defined(LAME_MSVC) && !defined(_USE_MATH_DEFINES)
#  define _USE_MATH_DEFINES
#endif

#include <math.h>

LAME_BEGIN

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
	return abs(x);
}

Sint32 labs(Sint32 x) {
#ifdef LAME_MSVC
	return (Sint32)::labs(x);
#else
	return (Sint32)labs(x);
#endif
}

Sint64 llabs(Sint64 x) {
#ifdef LAME_MSVC
	return ::llabs(x);
#else
	return llabs(x);
#endif
}

Float32 pow(Float32 x, Float32 y) {
	return powf(x, y);
}

Float64 pow(Float64 x, Float64 y) {
	return powd(x, y);
}

Float32 pow(Sint32 x, Sint32 y) {
	return powl(x, y);
}

Float64 pow(Sint64 x, Sint64 y) {
	return powll(x, y);
}

Float32 sqrt(Float32 x) {
	return sqrtf(x);
}

Float64 sqrt(Float64 x) {
	return sqrtd(x);
}

Float32 sqrt(Sint32 x) {
	return sqrtl(x);
}

Float64 sqrt(Sint64 x) {
	return sqrtll(x);
}

Float32 sqr(Float32 x) {
	return sqrf(x);
}

Float64 sqr(Float64 x) {
	return sqrd(x);
}

Sint32 sqr(Sint32 x) {
	return sqrl(x);
}

Sint64 sqr(Sint64 x) {
	return sqrll(x);
}

Float32 abs(Float32 x) {
	return fabs(x);
}

Float64 abs(Float64 x) {
	return dabs(x);
}

Sint32 abs(Sint32 x) {
	return labs(x);
}

Sint64 abs(Sint64 x) {
	return llabs(x);
}

LAME_END

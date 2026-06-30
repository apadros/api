#include <math.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"

program_local f32 PI = 3.14159265359f;

program_local f32 RadiansToDegrees(f32 rads) {
	return rads * PI / 180;
}

program_local f32 DegreesToRadias(f32 degs) {
	return degs * 180 / PI;
}

dll_export program_external vector GetTopRight(rectangle& r) {
	vector ret = {};
	ret.x = r.left + r.width;
	ret.y = r.bottom + r.height;
	return ret;
}

dll_export program_external f32 LERP(f32 min, f32 max, f32 perc) {
	Clamp(perc, 0.0f, 1.0f);
	return min + perc * (max - min);
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export vector vector::operator+(vector& v) {
	vector ret = {};
	ret.x = x + v.x;
	ret.y = y + v.y;
	return ret;
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export vector vector::operator-(vector& v) {
	vector ret = {};
	ret.x = x - v.x;
	ret.y = y - v.y;
	return ret;
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export void vector::operator+=(vector& v) {
	this->x += v.x;
	this->y += v.y;
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export void vector::operator-=(vector& v) {
	this->x -= v.x;
	this->y -= v.y;
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export vector vector::operator*(f32 f) {
	vector ret = {};
	ret.x = x * f;
	ret.y = y * f;
	return ret;
}

// Making this 'program_external' fails compilation when #including math.h. Go figure.
dll_export vector vector::operator/(f32 f) {
	vector ret = {};
	ret.x = x / f;
	ret.y = y / f;
	return ret;
}

dll_export program_external bool Overlap(f32 x0, f32 y0, f32 left1, f32 bottom1, f32 width1, f32 height1) {
	return x0 >= left1 && x0 <= left1 + width1 &&
				 y0 >= bottom1 && y0 <= bottom1 + height1;
}

dll_export program_external vector GetMiddle(rectangle r) {
	vector ret = {};
	ret.width = r.left + r.width / 2;
	ret.height = r.bottom + r.height / 2;
	return ret;
}

dll_export program_external rectangle CreateRectangle(f32 left, f32 bottom, f32 width, f32 height) {
	FunctionStart(rectangle());
	AssertInternal(width != 0);
	AssertInternal(height != 0);
	rectangle ret = {};
	ret.left = left;
	ret.bottom = bottom;
	ret.width = width;
	ret.height = height;
	FunctionEnd();
	return ret;
}

dll_export program_external f32 Sine(f32 degs) {
	auto rads = RadiansToDegrees(degs);
	auto ret = sinf(rads);
	return ret;
}

dll_export program_external f32 Cos(f32 degs) {
	auto rads = RadiansToDegrees(degs);
	auto ret = cosf(rads);
	return ret;
}

dll_export f32 RoundToNearestInteger(f32 value) {
	return round(value);
}
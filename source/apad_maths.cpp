#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"

dll_export vector GetTopRight(rectangle& r) {
	vector ret = {};
	ret.x = r.left + r.width;
	ret.y = r.bottom + r.height;
	return ret;
}

dll_export f32 LERP(f32 min, f32 max, f32 perc) {
	Clamp(perc, 0.0f, 1.0f);
	return min + perc * (max - min);
}

dll_export vector vector::operator+(vector& v) {
	vector ret = {};
	ret.x = x + v.x;
	ret.y = y + v.y;
	return ret;
}

dll_export vector vector::operator-(vector& v) {
	vector ret = {};
	ret.x = x - v.x;
	ret.y = y - v.y;
	return ret;
}

dll_export void vector::operator+=(vector& v) {
	this->x += v.x;
	this->y += v.y;
}

dll_export void vector::operator-=(vector& v) {
	this->x -= v.x;
	this->y -= v.y;
}

dll_export vector vector::operator*(f32 f) {
	vector ret = {};
	ret.x = x * f;
	ret.y = y * f;
	return ret;
}

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

dll_export rectangle CreateRectangle(f32 left, f32 bottom, f32 width, f32 height) {
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
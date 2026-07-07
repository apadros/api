#include <math.h>
#include "apad_base_types.h"
#include "apad_error_internal.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"

program_local f32 PI = 3.14159265359f;

program_local f32 RadiansToDegrees(f32 rads) {
	return rads / PI * 180;
}

program_local f32 DegreesToRadians(f32 degs) {
	return degs / 180 * PI;
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

dll_export program_external bool Overlap(f32 x0, f32 y0, f32 x1, f32 y1, f32 testDistance) {
	FunctionStart(false);
	AssertInternal(testDistance > 0);
	auto v = CreateVector(x1 - x0, y1 - y0);
	auto ret = Magnitude(v) <= testDistance;
	FunctionEnd();
	return ret;
}

dll_export program_external vector GetCenter(rectangle r) {
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

dll_export program_external rectangle CreateRectangle(vector pos, vector size) {
	FunctionStart(rectangle());
	auto ret = CreateRectangle(pos.x, pos.y, size.width, size.height);
	FunctionEnd();
	return ret;
}

dll_export program_external f32 Sine(f32 degs) {
	auto rads = DegreesToRadians(degs);
	auto ret = sinf(rads);
	return ret;
}

dll_export program_external f32 Cos(f32 degs) {
	auto rads = DegreesToRadians(degs);
	auto ret = cosf(rads);
	return ret;
}

dll_export program_external f32 RoundToNearestInteger(f32 value) {
	return round(value);
}

dll_export program_external f32 SquareRoot(f32 f) {
	if(f == 0.0f)
		return f;
	FunctionStart(Null);
	AssertInternal(f > 0);
	auto ret = sqrt(f);
	FunctionEnd();
	return ret;
}

dll_export program_external vector CreateVector(f32 x, f32 y) {
	vector ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

dll_export program_external f32 Magnitude(f32 f) {
	return f < 0 ? -f : f;
}

dll_export program_external f32 Magnitude(vector& v) {
	return SquareRoot(v.x * v.x + v.y * v.y);
}

dll_export program_external f32 Tan(f32 degs) {
	auto rads = DegreesToRadians(degs);
	auto ret = tanf(rads);
	return ret;
}

dll_export program_external f32 ArcTan(f32 f) {
	auto ret = atanf(f);
	return RadiansToDegrees(ret);
}

dll_export program_external f32 ArcSine(f32 f) {
	auto ret = asinf(f);
	return RadiansToDegrees(ret);
}

dll_export program_external f32 ArcCos(f32 f) {
	auto ret = acosf(f);
	return RadiansToDegrees(ret);
}
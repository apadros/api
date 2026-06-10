#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"

dll_export program_external bool Overlap(f32 x0, f32 y0, f32 left1, f32 bottom1, f32 width1, f32 height1) {
	return x0 >= left1 && x0 <= left1 + width1 &&
				 y0 >= bottom1 && y0 <= bottom1 + height1;
}

dll_export program_external size GetMiddle(rectangle r) {
	size ret = {};
	ret.width = r.left + r.width / 2;
	ret.height = r.bottom + r.height / 2;
	return ret;
}
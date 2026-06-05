#include "apad_base_types.h"
#include "apad_intrinsics.h"
#include "apad_maths.h"

dll_export program_external bool Overlap(ui16 x0, ui16 y0, ui16 left1, ui16 bottom1, ui16 width1, ui16 height1) {
	return x0 >= left1 && x0 <= left1 + width1 &&
				 y0 >= bottom1 && y0 <= bottom1 + height1;
}

dll_export program_external size GetMiddle(rectangle r) {
	size ret = {};
	ret.width = r.left + r.width / 2;
	ret.height = r.bottom + r.height / 2;
	return ret;
}
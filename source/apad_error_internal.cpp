#include <setjmp.h>
#include "apad_base_types.h"

jmp_buf JumpBuffer;
si8     ReturnFromAPI = -1; // Unset

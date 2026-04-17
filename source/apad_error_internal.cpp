#include <setjmp.h>
#include "apad_base_types.h"
#include "apad_intrinsics.h"

program_external jmp_buf JumpBuffer = {};
program_external si8     JumpBufferRefCounter = 0;

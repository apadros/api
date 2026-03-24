#include <setjmp.h>
#include "apad_base_types.h"

program_external jmp_buf JumpEnv;
program_external si8     ReturnFromAPI = -1; // Unset
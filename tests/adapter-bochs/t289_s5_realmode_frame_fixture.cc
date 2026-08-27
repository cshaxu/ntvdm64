#include "adapter-bochs/machine_facade.h"

#include <string.h>

int main(void)
{
  struct machine_facade_real_mode_frame before, candidate, after, stable;
  int result = 1;

  if (!machine_facade_machine_begin(0x100000u, 0x100000u)) return 1;
  machine_facade_apply_real_mode_entry(0x1111u, 0x1234u);
  if (!machine_facade_set_sp16(0x2222u) ||
      machine_facade_copy_real_mode_frame(&before) !=
        MACHINE_FACADE_REAL_MODE_FRAME_OK ||
      !machine_facade_real_mode_frame_valid(&before) ||
      before.execution_mode != MACHINE_FACADE_EXECUTION_MODE_REAL ||
      before.cs != 0x1111u || before.sp != 0x2222u ||
      before.ip != 0x1234u) {
    result = 2;
    goto done;
  }

  candidate = before;
  candidate.cs = 0x3333u;
  candidate.ss = 0x4444u;
  candidate.sp = 0x5555u;
  candidate.ip = 0x6666u;
  candidate.eflags ^= 1u;
  if (machine_facade_commit_real_mode_frame(&before, &candidate) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK ||
      machine_facade_copy_real_mode_frame(&after) !=
        MACHINE_FACADE_REAL_MODE_FRAME_OK || after.cs != candidate.cs ||
      after.ss != candidate.ss || after.sp != candidate.sp ||
      after.ip != candidate.ip || ((after.eflags ^ candidate.eflags) & 1u)) {
    result = 3;
    goto done;
  }

  stable = after;
  candidate = after;
  candidate.eflags ^= 0x80000000u;
  candidate.ip = 0x7777u;
  if (machine_facade_commit_real_mode_frame(&after, &candidate) !=
      MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_CHANGE ||
      machine_facade_copy_real_mode_frame(&after) !=
        MACHINE_FACADE_REAL_MODE_FRAME_OK || memcmp(&stable, &after,
          sizeof(after)) != 0) {
    result = 4;
    goto done;
  }

  candidate = before;
  candidate.ip = 0x8888u;
  if (machine_facade_commit_real_mode_frame(&before, &candidate) !=
      MACHINE_FACADE_REAL_MODE_FRAME_REJECTED_STALE) {
    result = 5;
    goto done;
  }
  result = 0;

done:
  if (!machine_facade_machine_cleanup() && result == 0) result = 6;
  return result;
}

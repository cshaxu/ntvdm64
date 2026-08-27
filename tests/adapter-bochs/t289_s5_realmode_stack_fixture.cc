#include "adapter-bochs/machine_facade.h"

extern "C" {
#include "session/session.h"
void DpmiPushRmInt(unsigned short interrupt_number);
void DpmiSimulateIretCF(void);
extern unsigned long RmBopFe;
}

#include <string.h>

static int machine_read(void *, uint32_t address, uint8_t *bytes,
    uint32_t byte_count)
{
  return machine_facade_memory_read(address, byte_count, bytes);
}

static int machine_write(void *, uint32_t address, const uint8_t *bytes,
    uint32_t byte_count)
{
  return machine_facade_memory_write(address, byte_count, bytes);
}

int main(void)
{
  session instance;
  machine_facade_real_mode_frame before, candidate, after, stable;
  uint8_t ivt[4] = { 0x01u, 0xefu, 0xcdu, 0xabu };
  uint8_t stack[6];
  int result = 1;

  if (!machine_facade_machine_begin(0x100000u, 0x100000u) ||
      !machine_facade_memory_write(0x84u, sizeof(ivt), ivt)) return 1;
  session_initialize(&instance, 1u);
  if (!session_activate(&instance) || !session_thread_bind(&instance) ||
      !session_guest_memory_begin(&instance, 0, machine_read, machine_write)) {
    result = 2;
    goto done_machine;
  }
  machine_facade_apply_real_mode_entry(0x0300u, 0x0400u);
  if (machine_facade_copy_real_mode_frame(&before) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK) { result = 3; goto done_session; }
  candidate = before;
  candidate.ss = 0x0100u;
  candidate.sp = 0x0200u;
  if (machine_facade_commit_real_mode_frame(&before, &candidate) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK) { result = 4; goto done_session; }
  before = candidate;
  RmBopFe = 0x13572468ul;
  DpmiPushRmInt(0x21u);
  if (machine_facade_copy_real_mode_frame(&after) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK || after.ss != before.ss ||
      after.sp != 0x01fau || after.cs != 0xabcdu || after.ip != 0xef01u ||
      !machine_facade_memory_read(0x11fau, sizeof(stack), stack) ||
      stack[0] != 0x68u || stack[1] != 0x24u || stack[2] != 0x57u ||
      stack[3] != 0x13u || stack[4] != (uint8_t)before.eflags ||
      stack[5] != (uint8_t)(before.eflags >> 8)) {
    result = 5;
    goto done_session;
  }
  candidate = after;
  candidate.eflags |= 1u;
  if (machine_facade_commit_real_mode_frame(&after, &candidate) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK) { result = 6; goto done_session; }
  DpmiSimulateIretCF();
  if (machine_facade_copy_real_mode_frame(&after) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK || after.cs != 0x1357u ||
      after.ip != 0x2468u || after.ss != before.ss || after.sp != before.sp ||
      ((after.eflags & 0xffffu) != ((before.eflags & 0xfffeu) | 1u))) {
    result = 7;
    goto done_session;
  }
  stable = after;
  DpmiPushRmInt(0xffffu);
  if (machine_facade_copy_real_mode_frame(&after) !=
      MACHINE_FACADE_REAL_MODE_FRAME_OK || memcmp(&stable, &after,
        sizeof(after)) != 0) { result = 8; goto done_session; }
  result = 0;

done_session:
  session_guest_memory_end(&instance);
  if (!session_thread_unbind(&instance) || !session_dispose(&instance))
    result = result == 0 ? 9 : result;
done_machine:
  if (!machine_facade_machine_cleanup() && result == 0) result = 10;
  return result;
}

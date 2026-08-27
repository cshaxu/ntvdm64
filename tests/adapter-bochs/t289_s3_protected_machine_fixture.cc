#include "adapter-bochs/machine_facade.h"
#include "adapter-mvdm-host-in/generic_ud_bridge.h"

#include <string.h>

static int stop_on_ud(void *context, const void *event, unsigned event_bytes,
  void *outcome, unsigned outcome_bytes)
{
  const struct runtime_generic_ud_event *source =
    (const struct runtime_generic_ud_event *)event;
  struct runtime_generic_ud_outcome *result =
    (struct runtime_generic_ud_outcome *)outcome;
  (void)context;
  if (source == 0 || source->magic != RUNTIME_GENERIC_UD_EVENT_MAGIC ||
      source->vector != 6u || outcome_bytes != sizeof(*result) ||
      event_bytes != sizeof(*source))
    return 0;
  memset(result, 0, sizeof(*result));
  result->abi_version = RUNTIME_GENERIC_UD_EVENT_VERSION;
  result->disposition = RUNTIME_GENERIC_UD_STOP;
  return 1;
}

static int enter_protected_stop(void)
{
  static const uint8_t gdt[] = {
    0x17, 0x00, 0x20, 0x08, 0x00, 0x00, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x9a, 0xcf, 0x00,
    0xff, 0xff, 0x00, 0x00, 0x00, 0x92, 0xcf, 0x00
  };
  static const uint8_t transition[] = {
    0x0f, 0x01, 0x16, 0x00, 0x08,             /* lgdt [0800h] */
    0x66, 0xb8, 0x01, 0x00, 0x00, 0x00,         /* mov eax, 1 */
    0x0f, 0x22, 0xc0,                           /* mov cr0, eax */
    0xea, 0x00, 0x09, 0x08, 0x00                /* far jmp 0008:0900 */
  };
  static const uint8_t stop[] = { 0x0f, 0x0b };
  if (!machine_facade_machine_begin(0x100000u, 0x100000u)) return 10;
  if (!machine_facade_memory_write(0x800u, sizeof(gdt), gdt)) return 11;
  if (!machine_facade_memory_write(0x700u, sizeof(transition), transition)) return 12;
  if (!machine_facade_memory_write(0x900u, sizeof(stop), stop)) return 13;
  if (!machine_facade_bind_opaque_callback(stop_on_ud, 0)) return 14;
  machine_facade_apply_real_mode_entry(0x70u, 0u);
  machine_facade_cpu_loop();
  return 1;
}

int main(void)
{
  struct machine_facade_protected_frame before, candidate, after, stable;
  struct machine_facade_protected_segment code;
  uint8_t bytes[2];
  int result = 1;

  result = enter_protected_stop();
  if (result != 1) goto done;
  if (machine_facade_copy_protected_frame(&before) !=
      MACHINE_FACADE_PROTECTED_FRAME_OK ||
      !machine_facade_protected_frame_valid(&before) ||
      before.execution_mode != MACHINE_FACADE_EXECUTION_MODE_PROTECTED ||
      before.cs != 0x08u) { result = 2; goto done; }
  if (machine_facade_copy_protected_segment(1u, &code) !=
      MACHINE_FACADE_PROTECTED_FRAME_OK || code.selector != 0x08u) {
    result = 3; goto done;
  }
  if (machine_facade_protected_span_transfer(1u, 1u, 0x900u,
      sizeof(bytes), bytes) != MACHINE_FACADE_PROTECTED_RANGE_OK ||
      bytes[0] != 0x0fu || bytes[1] != 0x0bu) { result = 4; goto done; }

  candidate = before;
  candidate.eax = 0x11223344u;
  candidate.eflags ^= 1u;
  if (machine_facade_commit_protected_frame(&before, &candidate) !=
      MACHINE_FACADE_PROTECTED_FRAME_OK ||
      machine_facade_copy_protected_frame(&after) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK ||
      after.eax != candidate.eax ||
      ((after.eflags ^ candidate.eflags) & 1u) != 0u) {
    result = 5; goto done;
  }

  stable = after;
  candidate = after;
  candidate.cs ^= 8u;
  if (machine_facade_commit_protected_frame(&after, &candidate) !=
      MACHINE_FACADE_PROTECTED_FRAME_REJECTED_CHANGE ||
      machine_facade_copy_protected_frame(&after) !=
        MACHINE_FACADE_PROTECTED_FRAME_OK || memcmp(&stable, &after,
          sizeof(after)) != 0) { result = 6; goto done; }
  candidate = before;
  candidate.ebx = 0x55667788u;
  if (machine_facade_commit_protected_frame(&before, &candidate) !=
      MACHINE_FACADE_PROTECTED_FRAME_REJECTED_STALE) { result = 7; goto done; }
  result = 0;

done:
  machine_facade_unbind_opaque_callback();
  if (!machine_facade_machine_cleanup() && result == 0) result = 8;
  return result;
}

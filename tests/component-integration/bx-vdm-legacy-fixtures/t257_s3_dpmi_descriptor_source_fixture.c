#include "opennt-bop/dpmi/dpmi_descriptor_source_shim.h"
#include "adapter-softpc/mechanical_action.h"
#include "adapter-softpc/protected_range_action.h"

#include <string.h>

void DpmiPassTableAddress(void);

static LDT_ENTRY input_descriptor;
static uint8_t published[sizeof(LDT_ENTRY)];
static uint64_t published_address;
static int protected_readable = 1;
static int guest_writable = 1;

void runtime_protected_range_action_v1_clear(
  struct runtime_protected_range_action_v1 *action)
{
  if (action == 0) return;
  memset(action, 0, sizeof(*action));
  action->magic = RUNTIME_PROTECTED_RANGE_ACTION_V1_MAGIC;
  action->abi_version = RUNTIME_PROTECTED_RANGE_ACTION_V1_VERSION;
  action->struct_bytes = sizeof(*action);
  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_INPUT;
}

uint32_t runtime_mantle_execute_protected_range_action_v1(
  struct runtime_protected_range_action_v1 *action)
{
  if (action == 0 || !protected_readable ||
      action->kind != RUNTIME_PROTECTED_RANGE_ACTION_V1_READ ||
      action->segment != 0u || action->offset != 0x100u ||
      action->byte_count != sizeof(input_descriptor))
    return RUNTIME_PROTECTED_RANGE_ACTION_V1_REJECTED_ACCESS;
  memcpy(action->bytes, &input_descriptor, sizeof(input_descriptor));
  action->status = RUNTIME_PROTECTED_RANGE_ACTION_V1_OK;
  return action->status;
}

int runtime_mantle_checked_ram_read_v1(uint64_t address, uint8_t *bytes,
  uint32_t byte_count)
{
  (void)address; (void)bytes; (void)byte_count;
  return 0;
}

int runtime_mantle_checked_ram_write_v1(uint64_t address,
  const uint8_t *bytes, uint32_t byte_count)
{
  if (!guest_writable || bytes == 0 || byte_count != sizeof(published)) return 0;
  published_address = address;
  memcpy(published, bytes, sizeof(published));
  return 1;
}

int main(void)
{
  runtime_cpu_state_v1 cpu;
  runtime_cpu_state_v1 observed;

  runtime_dpmi_startup_session_runtime_reset();
  if (!runtime_dpmi_startup_session_runtime_stage_selector_table(0x3000u)) return 1;
  /* Preserve the source lifecycle: staging the selGDT address is not its
   * publication.  DpmiPassTableAddress owns the original record update that
   * DpmiSetDescriptorEntry subsequently consumes. */
  DpmiPassTableAddress();
  runtime_cpu_state_v1_initialize(&cpu, RUNTIME_CPU_EXECUTION_PROTECTED);
  cpu.eax = 0x20u;
  cpu.ebx = 0x100u;
  cpu.ecx = 1u;
  cpu.es = 0x0cu;
  if (!runtime_dpmi_startup_session_runtime_stage_app(&cpu, 0u)) return 2;

  memset(&input_descriptor, 0, sizeof(input_descriptor));
  input_descriptor.LimitLow = 0xffffu;
  input_descriptor.BaseLow = 0x8000u;
  input_descriptor.HighWord.Bytes.BaseMid = 0xfeu;
  input_descriptor.HighWord.Bytes.BaseHi = 0x7fu;
  input_descriptor.HighWord.Bytes.Flags1 = 0x92u;
  input_descriptor.HighWord.Bytes.Flags2 = 0x40u;
  memset(published, 0, sizeof(published));
  DpmiSetDescriptorEntry();
  if (published_address != 0x3020u ||
      ((const LDT_ENTRY *)published)->LimitLow != 0x7000u ||
      !runtime_dpmi_startup_session_runtime_copy_cpu(&observed) ||
      (uint16_t)observed.eax != 0u) return 3;

  cpu.eax = 0x21u;
  if (!runtime_dpmi_startup_session_runtime_stage_app(&cpu, 0u)) return 4;
  published_address = 0u;
  DpmiSetDescriptorEntry();
  if (published_address != 0u ||
      !runtime_dpmi_startup_session_runtime_copy_cpu(&observed) ||
      (uint16_t)observed.eax != 0x21u) return 5;

  cpu.eax = 0x20u;
  if (!runtime_dpmi_startup_session_runtime_stage_app(&cpu, 0u)) return 6;
  protected_readable = 0;
  DpmiSetDescriptorEntry();
  if (!runtime_dpmi_startup_session_runtime_copy_cpu(&observed) ||
      (uint16_t)observed.eax != 0x20u) return 7;
  return 0;
}

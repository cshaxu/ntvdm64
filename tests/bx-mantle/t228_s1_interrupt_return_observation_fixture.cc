#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "adapter-bop/bx_ntvdm_generic_ud_bridge.h"
#include "adapter-softpc/bx_ntvdm_interrupt_return_observation_v1.h"

#include <string.h>

extern "C" int bx_ntvdm_mantle_generic_ud_bridge_v1(
  const struct bx_ntvdm_generic_ud_event_v1 *event,
  struct bx_ntvdm_generic_ud_outcome_v1 *outcome)
{
  if (event == 0 || outcome == 0 || event->vector != 6u) return 0;
  outcome->abi_version = BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION;
  outcome->disposition = BX_NTVDM_GENERIC_UD_STOP;
  bx_ntvdm_mantle_generic_ud_stop_observation_mark();
  return 1;
}

static void initialize_request(struct bx_ntvdm_finite_run_request *request)
{
  memset(request, 0, sizeof(*request));
  request->request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  request->entry_physical_address = 0u;
  request->entry_cs = 0u;
  request->instruction_tick_budget = 128u;
  request->ips = 1u;
}

static int exercise_real_iret16(void)
{
  struct bx_ntvdm_finite_run_request request;
  struct bx_ntvdm_interrupt_return_observation_v1_record record;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  request.entry_bytes[0x80] = 0x10u; request.entry_bytes[0x81] = 0x01u;
  request.entry_bytes[0x100] = 0xb8u; request.entry_bytes[0x101] = 0x34u;
  request.entry_bytes[0x102] = 0x12u; /* mov ax,1234 */
  request.entry_bytes[0x103] = 0xbbu; request.entry_bytes[0x104] = 0x78u;
  request.entry_bytes[0x105] = 0x56u; /* mov bx,5678 */
  request.entry_bytes[0x106] = 0xcdu; request.entry_bytes[0x107] = 0x20u;
  request.entry_bytes[0x108] = 0x0fu; request.entry_bytes[0x109] = 0x0bu;
  request.entry_bytes[0x110] = 0xcfu; /* iret */
  request.entry_byte_count = 0x111u;
  request.entry_eip = 0x100u;

  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_configure(0u)) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ||
      bx_ntvdm_mantle_interrupt_return_observation_v1_count() != 0u) return 2;

  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_configure(8u)) return 3;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ||
      bx_ntvdm_mantle_interrupt_return_observation_v1_count() != 1u) return 4;
  memset(&record, 0, sizeof(record));
  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_get(0u, &record) ||
      record.version != BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_VERSION ||
      record.execution_mode != 1u || record.operand_width != 16u ||
      record.cs != 0u || record.rip != 0x108u ||
      record.ax != 0x1234u || record.bx != 0x5678u) return 5;
  return 0;
}

static int exercise_real_iret32(void)
{
  struct bx_ntvdm_finite_run_request request;
  struct bx_ntvdm_interrupt_return_observation_v1_record record;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  /* Set a 16-bit real-mode stack, construct a 32-bit IRET frame, then 66 CF. */
  request.entry_bytes[0x100] = 0xb8u; request.entry_bytes[0x101] = 0x34u;
  request.entry_bytes[0x102] = 0x12u; /* mov ax,1234 */
  request.entry_bytes[0x103] = 0xbbu; request.entry_bytes[0x104] = 0x78u;
  request.entry_bytes[0x105] = 0x56u; /* mov bx,5678 */
  request.entry_bytes[0x106] = 0x8eu; request.entry_bytes[0x107] = 0xd0u; /* ss,ax */
  request.entry_bytes[0x108] = 0xbcu; request.entry_bytes[0x109] = 0x00u;
  request.entry_bytes[0x10a] = 0x09u; /* sp,0900 */
  request.entry_bytes[0x10b] = 0x66u; request.entry_bytes[0x10c] = 0x9cu; /* pushfd */
  request.entry_bytes[0x10d] = 0x66u; request.entry_bytes[0x10e] = 0x6au;
  request.entry_bytes[0x10f] = 0x00u; /* push dword 0 */
  request.entry_bytes[0x110] = 0x66u; request.entry_bytes[0x111] = 0x68u;
  request.entry_bytes[0x112] = 0x00u; request.entry_bytes[0x113] = 0x02u;
  request.entry_bytes[0x114] = 0x00u; request.entry_bytes[0x115] = 0x00u; /* push 200 */
  request.entry_bytes[0x116] = 0x66u; request.entry_bytes[0x117] = 0xcfu;
  request.entry_bytes[0x200] = 0x0fu; request.entry_bytes[0x201] = 0x0bu;
  request.entry_byte_count = 0x202u;
  request.entry_eip = 0x100u;

  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_configure(8u)) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ||
      bx_ntvdm_mantle_interrupt_return_observation_v1_count() != 1u) return 2;
  memset(&record, 0, sizeof(record));
  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_get(0u, &record) ||
      record.execution_mode != 1u || record.operand_width != 32u ||
      record.cs != 0u || record.rip != 0x200u ||
      record.ax != 0x1234u || record.bx != 0x5678u) return 3;
  return 0;
}

static int exercise_protected_mode_negative(void)
{
  struct bx_ntvdm_finite_run_request request;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  request.entry_bytes[0x100] = 0xfau;
  request.entry_bytes[0x101] = 0x0fu; request.entry_bytes[0x102] = 0x01u;
  request.entry_bytes[0x103] = 0x16u; request.entry_bytes[0x104] = 0x80u;
  request.entry_bytes[0x105] = 0x03u;
  request.entry_bytes[0x106] = 0x0fu; request.entry_bytes[0x107] = 0x01u;
  request.entry_bytes[0x108] = 0x1eu; request.entry_bytes[0x109] = 0x90u;
  request.entry_bytes[0x10a] = 0x03u;
  request.entry_bytes[0x10b] = 0x0fu; request.entry_bytes[0x10c] = 0x20u;
  request.entry_bytes[0x10d] = 0xc0u;
  request.entry_bytes[0x10e] = 0x83u; request.entry_bytes[0x10f] = 0xc8u;
  request.entry_bytes[0x110] = 0x01u;
  request.entry_bytes[0x111] = 0x0fu; request.entry_bytes[0x112] = 0x22u;
  request.entry_bytes[0x113] = 0xc0u;
  request.entry_bytes[0x114] = 0xeau; request.entry_bytes[0x115] = 0x00u;
  request.entry_bytes[0x116] = 0x02u; request.entry_bytes[0x117] = 0x08u;
  request.entry_bytes[0x118] = 0x00u;
  request.entry_bytes[0x200] = 0xb8u; request.entry_bytes[0x201] = 0x10u;
  request.entry_bytes[0x202] = 0x00u;
  request.entry_bytes[0x203] = 0x8eu; request.entry_bytes[0x204] = 0xd8u;
  request.entry_bytes[0x205] = 0x8eu; request.entry_bytes[0x206] = 0xc0u;
  request.entry_bytes[0x207] = 0x8eu; request.entry_bytes[0x208] = 0xd0u;
  request.entry_bytes[0x209] = 0xbcu; request.entry_bytes[0x20a] = 0x00u;
  request.entry_bytes[0x20b] = 0x09u;
  request.entry_bytes[0x20c] = 0xcdu; request.entry_bytes[0x20d] = 0x20u;
  request.entry_bytes[0x20e] = 0x0fu; request.entry_bytes[0x20f] = 0x0bu;
  request.entry_bytes[0x300] = 0xcfu; /* protected IRET */
  request.entry_bytes[0x408] = 0xffu; request.entry_bytes[0x409] = 0xffu;
  request.entry_bytes[0x40d] = 0x9au; request.entry_bytes[0x40e] = 0xcfu;
  request.entry_bytes[0x410] = 0xffu; request.entry_bytes[0x411] = 0xffu;
  request.entry_bytes[0x415] = 0x92u; request.entry_bytes[0x416] = 0xcfu;
  request.entry_bytes[0x380] = 0x17u; request.entry_bytes[0x382] = 0x00u;
  request.entry_bytes[0x383] = 0x04u;
  request.entry_bytes[0x390] = 0x07u; request.entry_bytes[0x391] = 0x02u;
  request.entry_bytes[0x392] = 0x00u; request.entry_bytes[0x393] = 0x05u;
  request.entry_bytes[0x600] = 0x00u; request.entry_bytes[0x601] = 0x03u;
  request.entry_bytes[0x602] = 0x08u; request.entry_bytes[0x605] = 0x8eu;
  request.entry_byte_count = 0x608u;
  request.entry_eip = 0x100u;

  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_configure(8u)) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP) return 2;
  return bx_ntvdm_mantle_interrupt_return_observation_v1_count() == 0u ? 0 : 3;
}

static int exercise_ring(void)
{
  struct bx_ntvdm_interrupt_return_observation_v1_record record;
  uint32_t index;
  if (!bx_ntvdm_mantle_interrupt_return_observation_v1_configure(3u)) return 1;
  memset(&record, 0, sizeof(record));
  record.version = BX_NTVDM_INTERRUPT_RETURN_OBSERVATION_V1_VERSION;
  for (index = 0u; index != 5u; ++index) {
    record.sequence = index;
    record.operand_width = (uint8_t)(16u + index);
    bx_ntvdm_mantle_interrupt_return_observation_v1_record(&record);
  }
  if (bx_ntvdm_mantle_interrupt_return_observation_v1_count() != 3u) return 2;
  for (index = 0u; index != 3u; ++index) {
    memset(&record, 0, sizeof(record));
    if (!bx_ntvdm_mantle_interrupt_return_observation_v1_get(index, &record) ||
        record.sequence != index + 2u || record.operand_width != 18u + index) return 3;
  }
  bx_ntvdm_mantle_interrupt_return_observation_v1_reset();
  return bx_ntvdm_mantle_interrupt_return_observation_v1_count() == 0u ? 0 : 4;
}

int main(void)
{
  int result = exercise_real_iret16();
  if (result != 0) return result;
  result = exercise_real_iret32();
  if (result != 0) return 20 + result;
  result = exercise_protected_mode_negative();
  if (result != 0) return 40 + result;
  result = exercise_ring();
  return result == 0 ? 0 : 60 + result;
}

#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-mantle/bx_ntvdm_generic_ud_bridge.h"
#include "bx-mantle/bx_ntvdm_software_interrupt_observation_v1.h"

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
  request->stop_on_ud_fixture = 1u;
}

static int exercise_real_mode(void)
{
  struct bx_ntvdm_finite_run_request request;
  struct bx_ntvdm_software_interrupt_observation_v1_record record;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  /* IVT 20h enters a second neutral INT 21h; its handler reaches UD2. */
  request.entry_bytes[0x80] = 0x10u;
  request.entry_bytes[0x81] = 0x01u;
  request.entry_bytes[0x84] = 0x20u;
  request.entry_bytes[0x85] = 0x01u;
  request.entry_bytes[0x100] = 0xcdu;
  request.entry_bytes[0x101] = 0x20u;
  request.entry_bytes[0x110] = 0xcdu;
  request.entry_bytes[0x111] = 0x21u;
  request.entry_bytes[0x120] = 0x0fu;
  request.entry_bytes[0x121] = 0x0bu;
  request.entry_byte_count = 0x122u;
  request.entry_eip = 0x100u;

  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_configure(0u)) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ||
      bx_ntvdm_mantle_software_interrupt_observation_v1_count() != 0u) return 2;

  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_configure(8u)) return 3;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ||
      bx_ntvdm_mantle_software_interrupt_observation_v1_count() != 2u) return 4;
  memset(&record, 0, sizeof(record));
  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_get(0u, &record) ||
      record.version != BX_NTVDM_SOFTWARE_INTERRUPT_OBSERVATION_V1_VERSION ||
      record.vector != 0x20u || record.execution_mode != 1u ||
      record.cs != 0u || record.rip != 0x102u) return 5;
  memset(&record, 0, sizeof(record));
  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_get(1u, &record) ||
      record.vector != 0x21u || record.execution_mode != 1u ||
      record.cs != 0u || record.rip != 0x112u) return 6;
  return 0;
}

static int exercise_protected_mode_negative(void)
{
  struct bx_ntvdm_finite_run_request request;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  /* Real-mode setup installs GDT/IDT, enters PE, then protected INT 20h. */
  request.entry_bytes[0x100] = 0xfau; /* cli */
  request.entry_bytes[0x101] = 0x0fu; request.entry_bytes[0x102] = 0x01u;
  request.entry_bytes[0x103] = 0x16u; request.entry_bytes[0x104] = 0x80u;
  request.entry_bytes[0x105] = 0x03u; /* lgdt [0380] */
  request.entry_bytes[0x106] = 0x0fu; request.entry_bytes[0x107] = 0x01u;
  request.entry_bytes[0x108] = 0x1eu; request.entry_bytes[0x109] = 0x90u;
  request.entry_bytes[0x10a] = 0x03u; /* lidt [0390] */
  request.entry_bytes[0x10b] = 0x0fu; request.entry_bytes[0x10c] = 0x20u;
  request.entry_bytes[0x10d] = 0xc0u; /* mov eax, cr0 */
  request.entry_bytes[0x10e] = 0x83u; request.entry_bytes[0x10f] = 0xc8u;
  request.entry_bytes[0x110] = 0x01u; /* or eax, 1 */
  request.entry_bytes[0x111] = 0x0fu; request.entry_bytes[0x112] = 0x22u;
  request.entry_bytes[0x113] = 0xc0u; /* mov cr0, eax */
  request.entry_bytes[0x114] = 0xeau; request.entry_bytes[0x115] = 0x00u;
  request.entry_bytes[0x116] = 0x02u; request.entry_bytes[0x117] = 0x08u;
  request.entry_bytes[0x118] = 0x00u; /* jmp 8:0200 */
  request.entry_bytes[0x200] = 0xb8u; request.entry_bytes[0x201] = 0x10u;
  request.entry_bytes[0x202] = 0x00u; /* mov ax, 10 */
  request.entry_bytes[0x203] = 0x8eu; request.entry_bytes[0x204] = 0xd8u; /* ds */
  request.entry_bytes[0x205] = 0x8eu; request.entry_bytes[0x206] = 0xc0u; /* es */
  request.entry_bytes[0x207] = 0x8eu; request.entry_bytes[0x208] = 0xd0u; /* ss */
  request.entry_bytes[0x209] = 0xbcu; request.entry_bytes[0x20a] = 0x00u;
  request.entry_bytes[0x20b] = 0x09u; /* sp */
  request.entry_bytes[0x20c] = 0xcdu; request.entry_bytes[0x20d] = 0x20u;
  request.entry_bytes[0x20e] = 0x0fu; request.entry_bytes[0x20f] = 0x0bu;
  request.entry_bytes[0x300] = 0x0fu; request.entry_bytes[0x301] = 0x0bu;
  /* 24-byte GDT at 0400: null, flat code 8, flat data 10. */
  request.entry_bytes[0x408] = 0xffu; request.entry_bytes[0x409] = 0xffu;
  request.entry_bytes[0x40d] = 0x9au; request.entry_bytes[0x40e] = 0xcfu;
  request.entry_bytes[0x410] = 0xffu; request.entry_bytes[0x411] = 0xffu;
  request.entry_bytes[0x415] = 0x92u; request.entry_bytes[0x416] = 0xcfu;
  request.entry_bytes[0x380] = 0x17u; request.entry_bytes[0x382] = 0x00u;
  request.entry_bytes[0x383] = 0x04u; /* GDTR limit/base */
  request.entry_bytes[0x390] = 0x07u; request.entry_bytes[0x391] = 0x02u;
  request.entry_bytes[0x392] = 0x00u; request.entry_bytes[0x393] = 0x05u; /* IDTR */
  /* IDT 20h gate: offset 0300, selector 8, present 32-bit interrupt gate. */
  request.entry_bytes[0x600] = 0x00u; request.entry_bytes[0x601] = 0x03u;
  request.entry_bytes[0x602] = 0x08u; request.entry_bytes[0x605] = 0x8eu;
  request.entry_byte_count = 0x608u;
  request.entry_eip = 0x100u;

  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_configure(8u)) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP) return 2;
  return bx_ntvdm_mantle_software_interrupt_observation_v1_count() == 0u ? 0 : 3;
}

static int exercise_ring(void)
{
  struct bx_ntvdm_software_interrupt_observation_v1_record record;
  uint32_t i;

  if (!bx_ntvdm_mantle_software_interrupt_observation_v1_configure(3u)) return 1;
  memset(&record, 0, sizeof(record));
  record.version = BX_NTVDM_SOFTWARE_INTERRUPT_OBSERVATION_V1_VERSION;
  for (i = 0u; i != 5u; ++i) {
    record.sequence = i;
    record.vector = (uint8_t)(0x40u + i);
    bx_ntvdm_mantle_software_interrupt_observation_v1_record(&record);
  }
  if (bx_ntvdm_mantle_software_interrupt_observation_v1_count() != 3u) return 2;
  for (i = 0u; i != 3u; ++i) {
    memset(&record, 0, sizeof(record));
    if (!bx_ntvdm_mantle_software_interrupt_observation_v1_get(i, &record) ||
        record.sequence != i + 2u || record.vector != 0x42u + i) return 3;
  }
  bx_ntvdm_mantle_software_interrupt_observation_v1_reset();
  return bx_ntvdm_mantle_software_interrupt_observation_v1_count() == 0u ? 0 : 4;
}

int main(void)
{
  int result = exercise_real_mode();
  if (result != 0) return result;
  result = exercise_protected_mode_negative();
  if (result != 0) return 20 + result;
  result = exercise_ring();
  return result == 0 ? 0 : 40 + result;
}
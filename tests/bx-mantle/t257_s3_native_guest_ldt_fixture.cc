#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "adapter-softpc/bx_ntvdm_generic_ud_bridge.h"

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
  request->entry_eip = 0x100u;
  request->entry_byte_count = 0x800u;
  request->instruction_tick_budget = 128u;
  request->ips = 1u;
  request->stop_on_ud_fixture = 1u;
}

int main(void)
{
  struct bx_ntvdm_finite_run_request request;
  bx_ntvdm_finite_run_status status;

  initialize_request(&request);
  /* Real-mode setup: load a guest-memory GDT, enter PE, far-jump to code. */
  request.entry_bytes[0x100] = 0xfau; /* cli */
  request.entry_bytes[0x101] = 0x0fu; request.entry_bytes[0x102] = 0x01u;
  request.entry_bytes[0x103] = 0x16u; request.entry_bytes[0x104] = 0x80u;
  request.entry_bytes[0x105] = 0x03u; /* lgdt [0380] */
  request.entry_bytes[0x106] = 0x0fu; request.entry_bytes[0x107] = 0x20u;
  request.entry_bytes[0x108] = 0xc0u; /* mov eax, cr0 */
  request.entry_bytes[0x109] = 0x83u; request.entry_bytes[0x10a] = 0xc8u;
  request.entry_bytes[0x10b] = 0x01u; /* or eax, 1 */
  request.entry_bytes[0x10c] = 0x0fu; request.entry_bytes[0x10d] = 0x22u;
  request.entry_bytes[0x10e] = 0xc0u; /* mov cr0, eax */
  request.entry_bytes[0x10f] = 0xeau; request.entry_bytes[0x110] = 0x00u;
  request.entry_bytes[0x111] = 0x02u; request.entry_bytes[0x112] = 0x08u;
  request.entry_bytes[0x113] = 0x00u; /* jmp 8:0200 */

  /* Protected code: LLDT reads GDT entry 18h, then DS reads LDT entry 0ch.
   * Reaching UD2 proves both descriptor reads came from ordinary guest RAM. */
  request.entry_bytes[0x200] = 0x66u; request.entry_bytes[0x201] = 0xb8u;
  request.entry_bytes[0x202] = 0x18u; request.entry_bytes[0x203] = 0x00u; /* mov ax, 18 */
  request.entry_bytes[0x204] = 0x0fu; request.entry_bytes[0x205] = 0x00u;
  request.entry_bytes[0x206] = 0xd0u; /* lldt ax */
  request.entry_bytes[0x207] = 0x66u; request.entry_bytes[0x208] = 0xb8u;
  request.entry_bytes[0x209] = 0x0cu; request.entry_bytes[0x20a] = 0x00u; /* mov ax, 0c */
  request.entry_bytes[0x20b] = 0x8eu; request.entry_bytes[0x20c] = 0xd8u; /* mov ds, ax */
  request.entry_bytes[0x20d] = 0x0fu; request.entry_bytes[0x20e] = 0x0bu; /* ud2 */

  /* GDT at 0400: null, flat code 08, flat data 10, LDT at physical 0700. */
  request.entry_bytes[0x408] = 0xffu; request.entry_bytes[0x409] = 0xffu;
  request.entry_bytes[0x40d] = 0x9au; request.entry_bytes[0x40e] = 0xcfu;
  request.entry_bytes[0x410] = 0xffu; request.entry_bytes[0x411] = 0xffu;
  request.entry_bytes[0x415] = 0x92u; request.entry_bytes[0x416] = 0xcfu;
  request.entry_bytes[0x418] = 0xffu; request.entry_bytes[0x419] = 0x07u;
  request.entry_bytes[0x41a] = 0x00u; request.entry_bytes[0x41b] = 0x07u;
  request.entry_bytes[0x41d] = 0x82u; /* present system LDT descriptor */
  request.entry_bytes[0x380] = 0x1fu; request.entry_bytes[0x382] = 0x00u;
  request.entry_bytes[0x383] = 0x04u; /* GDTR: limit 1f, base 0400 */

  /* LDT at 0700: null, 16-bit present writable data descriptor at index 1. */
  request.entry_bytes[0x708] = 0xffu; request.entry_bytes[0x709] = 0xffu;
  request.entry_bytes[0x70d] = 0x92u;

  status = bx_ntvdm_run_finite_bare_bytes(&request);
  return status == BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ? 0 : 1;
}

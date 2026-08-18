/////////////////////////////////////////////////////////////////////////
//
// Source-shaped, generic real-mode witness for the post-50:36 NTDOS
// $Exec transfer.  It contains no BOP, guest artifact, adapter input or
// device contract.  The byte program prepares the same register roles as
// msproc.asm, then executes CLI; MOV SS; MOV SP; STI; PUSH DS; PUSH SI;
// MOV ES,DX; MOV DS,DX; MOV AX,BX; RETF.  The returned-to code records
// only its architectural result in ordinary fixture RAM before HLT.
//
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"

#include <string.h>

int main()
{
  static const Bit8u transfer[] = {
    0xb8, 0x22, 0x22,       // mov ax,2222; DS = entry segment
    0x8e, 0xd8,             // mov ds,ax
    0xbe, 0x00, 0x01,       // mov si,0100; child IP
    0xb8, 0x00, 0x30,       // mov ax,3000; child SS
    0xbf, 0xfe, 0xff,       // mov di,fffe; child SP
    0xba, 0x00, 0x40,       // mov dx,4000; child PDB segment
    0xbb, 0xef, 0xbe,       // mov bx,beef; child initial AX
    0xfa,                   // cli
    0x8e, 0xd0,             // mov ss,ax
    0x89, 0xfc,             // mov sp,di
    0xfb,                   // sti
    0x1e,                   // push ds
    0x56,                   // push si
    0x8e, 0xc2,             // mov es,dx
    0x8e, 0xda,             // mov ds,dx
    0x89, 0xd8,             // mov ax,bx
    0xcb                    // retf
  };
  static const Bit8u child[] = {
    0xa3, 0x00, 0x02,       // [0200] = AX
    0x8c, 0xd8, 0xa3, 0x02, 0x02, // [0202] = DS
    0x8c, 0xc0, 0xa3, 0x04, 0x02, // [0204] = ES
    0x8c, 0xd0, 0xa3, 0x06, 0x02, // [0206] = SS
    0x89, 0xe0, 0xa3, 0x08, 0x02, // [0208] = SP
    0xf4
  };
  static const Bit8u expected_stack[] = { 0x00, 0x01, 0x22, 0x22 };
  static const Bit8u expected_result[] = {
    0xef, 0xbe, 0x00, 0x40, 0x00, 0x40, 0x00, 0x30, 0xfe, 0xff
  };
  bx_ntvdm_finite_run_request request;
  bx_ntvdm_finite_run_terminal_snapshot terminal;
  bx_ntvdm_finite_run_status status;

  memset(&request, 0, sizeof(request));
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, transfer, sizeof(transfer));
  request.entry_byte_count = sizeof(transfer);
  request.entry_physical_address = 0x10000;
  request.entry_cs = 0x1000;
  request.entry_eip = 0;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.has_preentry_action = 1;
  bx_ntvdm_mechanical_action_v1_clear(&request.preentry_action);
  request.preentry_action.action_id = 1;
  request.preentry_action.kind = BX_NTVDM_MECHANICAL_ACTION_V1_WRITE;
  request.preentry_action.range_count = 1;
  request.preentry_action.payload_bytes = sizeof(child);
  request.preentry_action.ranges[0].physical_address = 0x22320;
  request.preentry_action.ranges[0].byte_count = sizeof(child);
  memcpy(request.preentry_action.payload, child, sizeof(child));
  request.capture_terminal_snapshot = 1;

  if (!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(
        0x3fffa, sizeof(expected_stack))) return 1;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET ||
      !bx_ntvdm_finite_run_terminal_snapshot_get(&terminal) ||
      terminal.cs != 0x2222 || terminal.eip != 0x118 ||
      terminal.captured_byte_count != sizeof(expected_stack) ||
      memcmp(terminal.captured_bytes, expected_stack, sizeof(expected_stack)))
    return 2;

  if (!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(
        0x40200, sizeof(expected_result))) return 3;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  if (status != BX_NTVDM_FINITE_RUN_COMPLETED_BUDGET ||
      !bx_ntvdm_finite_run_terminal_snapshot_get(&terminal) ||
      terminal.captured_byte_count != sizeof(expected_result) ||
      memcmp(terminal.captured_bytes, expected_result, sizeof(expected_result)))
    return 4;

  request.entry_physical_address = 0x100000;
  request.entry_byte_count = 1;
  return bx_ntvdm_run_finite_bare_bytes(&request) ==
    BX_NTVDM_FINITE_RUN_REJECTED_INPUT ? 0 : 5;
}

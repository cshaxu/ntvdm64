/////////////////////////////////////////////////////////////////////////
// Local exception-seam record.  See docs/etc/research/
// adapter-external-intrusion-exceptions.md, BX-UD-001.
/////////////////////////////////////////////////////////////////////////

#ifndef BX_NTVDM_EXCEPTION_INTERCEPT_H
#define BX_NTVDM_EXCEPTION_INTERCEPT_H

#include "bx_ntvdm_cpu_state_abi.h"
#include "bx_ntvdm_cpu_result_v2.h"
#include "bx_ntvdm_instruction_window_abi.h"

#define BX_NTVDM_EXCEPTION_INTERCEPT_ABI_VERSION 1

enum bx_ntvdm_exception_disposition {
  BX_NTVDM_EXCEPTION_PASS_THROUGH = 0,
  BX_NTVDM_EXCEPTION_RESUME = 1,
  BX_NTVDM_EXCEPTION_STOP = 2
};

struct bx_ntvdm_exception_request {
  Bit32u abi_version;
  Bit32u cpu_id;
  Bit32u vector;
  Bit32u error_code;
  Bit64u fault_rip;
  const bx_ntvdm_cpu_state_v1 *cpu_state;
  const bx_ntvdm_instruction_window_v1 *instruction_window;
};

struct bx_ntvdm_exception_response {
  Bit32u abi_version;
  Bit32u disposition;
  Bit64u resume_rip;
  bx_ntvdm_cpu_result_v2 cpu_result;
};

typedef bx_bool (*bx_ntvdm_exception_interceptor_t)(
  const bx_ntvdm_exception_request *request,
  bx_ntvdm_exception_response *response);

void bx_ntvdm_set_exception_interceptor(
  bx_ntvdm_exception_interceptor_t interceptor);

#endif

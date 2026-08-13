#ifndef BX_NTVDM_PRINTER_UNAVAILABLE_SERVICE_H
#define BX_NTVDM_PRINTER_UNAVAILABLE_SERVICE_H
#include "bx_ntvdm_exception_abi.h"
#include "bx_ntvdm_cpu_result_v2.h"
int bx_ntvdm_printer_unavailable_service_v1_dispatch(const bx_ntvdm_exception_event_v1 *, const bx_ntvdm_cpu_state_v1 *, const bx_ntvdm_instruction_window_v1 *, bx_ntvdm_cpu_result_v2 *);
#endif

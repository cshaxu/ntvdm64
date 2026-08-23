#ifndef BX_NTVDM_BOP_SHIM_DPMI_STARTUP_SESSION_SHIM_H
#define BX_NTVDM_BOP_SHIM_DPMI_STARTUP_SESSION_SHIM_H

/*
 * T257 S3 source-derived seam for OpenNT dpmi32.c's DpmiInitDosx,
 * DpmiInitApp and DpmiPassTableAddress.  The original source received
 * Sim32GetVDMPointer results and retained host pointers in globals.  Those
 * pointers cannot cross the current x86/x64 boundary.  This record preserves
 * exactly the DOSX shared-data field order as fixed-width guest-linear values.
 * It does not expose descriptor data or emulate protected-mode execution.
 */

#include <stdint.h>

#include "bx_ntvdm_cpu_state_abi.h"

#define BX_NTVDM_DPMI_STARTUP_SESSION_MAGIC UINT32_C(0x42584453)
#define BX_NTVDM_DPMI_STARTUP_SESSION_VERSION UINT32_C(1)
#define BX_NTVDM_DPMI_STARTUP_SESSION_SHARED_DATA_BYTES UINT32_C(58)

enum bx_ntvdm_dpmi_startup_session_status {
  BX_NTVDM_DPMI_STARTUP_SESSION_OK = 0u,
  BX_NTVDM_DPMI_STARTUP_SESSION_REJECTED_INPUT,
  BX_NTVDM_DPMI_STARTUP_SESSION_REJECTED_CPU_MODE,
  BX_NTVDM_DPMI_STARTUP_SESSION_GUEST_READ_FAILED
};

struct bx_ntvdm_dpmi_startup_session {
  uint32_t magic;
  uint32_t abi_version;
  uint32_t struct_bytes;
  uint32_t initialized;
  uint32_t shared_data_linear;
  uint16_t dosx_stack_segment;
  uint16_t dosx_stack_frame_size;
  uint32_t small_xlat_buffer_linear;
  uint32_t large_xlat_buffer_linear;
  uint32_t dosx_stack_frame_pointer_linear;
  uint32_t rm_bop_fe;
  uint16_t dosx_rm_code_segment;
  uint16_t dosx_pm_data_selector;
  uint16_t dosx_rm_code_selector;
  uint16_t reserved0;
  uint32_t dosx_dta_buffer_linear;
  uint32_t dosx_segment_to_selector;
  uint32_t dosx_fault_handler_iret;
  uint32_t dosx_fault_handler_iretd;
  uint32_t dosx_int_handler_iret;
  uint32_t dosx_int_handler_iretd;
  uint32_t dosx_iret;
  uint32_t dosx_iretd;
  uint32_t selector_table_linear;
  uint16_t current_app_flags;
  uint16_t current_psp_selector;
  uint16_t current_dta_selector;
  uint16_t current_dta_offset;
  uint32_t current_dta_linear;
};

#ifdef __cplusplus
extern "C" {
#endif

void bx_ntvdm_dpmi_startup_session_clear(
  struct bx_ntvdm_dpmi_startup_session *session);
int bx_ntvdm_dpmi_startup_session_valid(
  const struct bx_ntvdm_dpmi_startup_session *session);
uint32_t bx_ntvdm_dpmi_startup_session_initialize_dosx(
  struct bx_ntvdm_dpmi_startup_session *session, uint32_t shared_data_linear);
uint32_t bx_ntvdm_dpmi_startup_session_initialize_app(
  struct bx_ntvdm_dpmi_startup_session *session,
  const bx_ntvdm_cpu_state_v1 *cpu_state, uint32_t current_dta_linear);
uint32_t bx_ntvdm_dpmi_startup_session_publish_selector_table(
  struct bx_ntvdm_dpmi_startup_session *session, uint32_t selector_table_linear);

void bx_ntvdm_dpmi_startup_session_runtime_reset(void);
int bx_ntvdm_dpmi_startup_session_runtime_stage_dosx(uint32_t shared_data_linear);
int bx_ntvdm_dpmi_startup_session_runtime_stage_app(
  const bx_ntvdm_cpu_state_v1 *cpu_state, uint32_t current_dta_linear);
int bx_ntvdm_dpmi_startup_session_runtime_stage_selector_table(
  uint32_t selector_table_linear);
int bx_ntvdm_dpmi_startup_session_runtime_stage_dispatch(
  const bx_ntvdm_cpu_state_v1 *cpu_state, uint32_t index);
int bx_ntvdm_dpmi_startup_session_runtime_take_dispatch(uint32_t *index);
int bx_ntvdm_dpmi_startup_session_runtime_copy_cpu(
  bx_ntvdm_cpu_state_v1 *cpu_state);
void bx_ntvdm_dpmi_startup_session_runtime_set_ax(uint16_t value);
void bx_ntvdm_dpmi_startup_session_runtime_advance_ip(uint32_t bytes);
void bx_ntvdm_dpmi_startup_session_runtime_initialize_dosx(void);
void bx_ntvdm_dpmi_startup_session_runtime_initialize_app(void);
void bx_ntvdm_dpmi_startup_session_runtime_publish_selector_table(void);
const struct bx_ntvdm_dpmi_startup_session *
  bx_ntvdm_dpmi_startup_session_runtime_current(void);

#ifdef __cplusplus
}
#endif

#endif

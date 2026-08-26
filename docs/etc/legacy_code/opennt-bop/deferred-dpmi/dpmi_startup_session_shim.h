#ifndef RUNTIME_OPENNT_BOP_DPMI_STARTUP_SESSION_SHIM_H
#define RUNTIME_OPENNT_BOP_DPMI_STARTUP_SESSION_SHIM_H

/*
 * T257 S3 source-derived seam for OpenNT dpmi32.c's DpmiInitDosx,
 * DpmiInitApp and DpmiPassTableAddress.  The original source received
 * Sim32GetVDMPointer results and retained host pointers in globals.  Those
 * pointers cannot cross the current x86/x64 boundary.  This record preserves
 * exactly the DOSX shared-data field order as fixed-width guest-linear values.
 * It does not expose descriptor data or emulate protected-mode execution.
 */

#include <stdint.h>

#include "adapter-softpc/cpu_state_abi.h"

#define RUNTIME_DPMI_STARTUP_SESSION_MAGIC UINT32_C(0x42584453)
#define RUNTIME_DPMI_STARTUP_SESSION_VERSION UINT32_C(1)
#define RUNTIME_DPMI_STARTUP_SESSION_SHARED_DATA_BYTES UINT32_C(58)

enum runtime_dpmi_startup_session_status {
  RUNTIME_DPMI_STARTUP_SESSION_OK = 0u,
  RUNTIME_DPMI_STARTUP_SESSION_REJECTED_INPUT,
  RUNTIME_DPMI_STARTUP_SESSION_REJECTED_CPU_MODE,
  RUNTIME_DPMI_STARTUP_SESSION_GUEST_READ_FAILED
};

struct runtime_dpmi_startup_session {
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

void runtime_dpmi_startup_session_clear(
  struct runtime_dpmi_startup_session *session);
int runtime_dpmi_startup_session_valid(
  const struct runtime_dpmi_startup_session *session);
uint32_t runtime_dpmi_startup_session_initialize_dosx(
  struct runtime_dpmi_startup_session *session, uint32_t shared_data_linear);
uint32_t runtime_dpmi_startup_session_initialize_app(
  struct runtime_dpmi_startup_session *session,
  const runtime_cpu_state *cpu_state, uint32_t current_dta_linear);
uint32_t runtime_dpmi_startup_session_publish_selector_table(
  struct runtime_dpmi_startup_session *session, uint32_t selector_table_linear);

void runtime_dpmi_startup_session_runtime_reset(void);
int runtime_dpmi_startup_session_runtime_stage_dosx(uint32_t shared_data_linear);
int runtime_dpmi_startup_session_runtime_stage_app(
  const runtime_cpu_state *cpu_state, uint32_t current_dta_linear);
int runtime_dpmi_startup_session_runtime_stage_selector_table(
  uint32_t selector_table_linear);
int runtime_dpmi_startup_session_runtime_stage_dispatch(
  const runtime_cpu_state *cpu_state, uint32_t index);
int runtime_dpmi_startup_session_runtime_take_dispatch(uint32_t *index);
int runtime_dpmi_startup_session_runtime_copy_cpu(
  runtime_cpu_state *cpu_state);
void runtime_dpmi_startup_session_runtime_set_ax(uint16_t value);
void runtime_dpmi_startup_session_runtime_advance_ip(uint32_t bytes);
void runtime_dpmi_startup_session_runtime_initialize_dosx(void);
void runtime_dpmi_startup_session_runtime_initialize_app(void);
void runtime_dpmi_startup_session_runtime_publish_selector_table(void);
const struct runtime_dpmi_startup_session *
  runtime_dpmi_startup_session_runtime_current(void);

#ifdef __cplusplus
}
#endif

#endif

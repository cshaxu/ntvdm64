#include "dpmi_startup_session_shim.h"

#include "bx-mantle/bx_ntvdm_mechanical_action_v1.h"

#include <string.h>

static uint16_t read_u16(const uint8_t *bytes, uint32_t offset)
{
  return (uint16_t)((uint16_t)bytes[offset] | ((uint16_t)bytes[offset + 1u] << 8));
}

static uint32_t read_u32(const uint8_t *bytes, uint32_t offset)
{
  return (uint32_t)bytes[offset] | ((uint32_t)bytes[offset + 1u] << 8) |
    ((uint32_t)bytes[offset + 2u] << 16) | ((uint32_t)bytes[offset + 3u] << 24);
}

void bx_ntvdm_dpmi_startup_session_clear(
  struct bx_ntvdm_dpmi_startup_session *session)
{
  if (session == 0) return;
  memset(session, 0, sizeof(*session));
  session->magic = BX_NTVDM_DPMI_STARTUP_SESSION_MAGIC;
  session->abi_version = BX_NTVDM_DPMI_STARTUP_SESSION_VERSION;
  session->struct_bytes = sizeof(*session);
}

int bx_ntvdm_dpmi_startup_session_valid(
  const struct bx_ntvdm_dpmi_startup_session *session)
{
  return session != 0 &&
    session->magic == BX_NTVDM_DPMI_STARTUP_SESSION_MAGIC &&
    session->abi_version == BX_NTVDM_DPMI_STARTUP_SESSION_VERSION &&
    session->struct_bytes == sizeof(*session) && session->reserved0 == 0u;
}

uint32_t bx_ntvdm_dpmi_startup_session_initialize_dosx(
  struct bx_ntvdm_dpmi_startup_session *session, uint32_t shared_data_linear)
{
  uint8_t shared[BX_NTVDM_DPMI_STARTUP_SESSION_SHARED_DATA_BYTES];
  if (!bx_ntvdm_dpmi_startup_session_valid(session) || shared_data_linear == 0u)
    return BX_NTVDM_DPMI_STARTUP_SESSION_REJECTED_INPUT;
  if (!bx_ntvdm_mantle_checked_ram_read_v1(shared_data_linear, shared, sizeof(shared)))
    return BX_NTVDM_DPMI_STARTUP_SESSION_GUEST_READ_FAILED;

  /* DIVERGENCE (T257 S3): this is DpmiInitDosx's original field order, but
   * each Sim32GetVDMPointer result is retained as a fixed-width guest-linear
   * address instead of a raw host pointer. */
  session->shared_data_linear = shared_data_linear;
  session->dosx_stack_segment = read_u16(shared, 0u);
  session->small_xlat_buffer_linear = read_u32(shared, 2u);
  session->large_xlat_buffer_linear = read_u32(shared, 6u);
  session->dosx_stack_frame_pointer_linear = read_u32(shared, 10u);
  session->dosx_stack_frame_size = read_u16(shared, 14u);
  session->rm_bop_fe = read_u32(shared, 16u);
  session->dosx_rm_code_segment = read_u16(shared, 20u);
  session->dosx_dta_buffer_linear = read_u32(shared, 22u);
  session->dosx_pm_data_selector = read_u16(shared, 26u);
  session->dosx_rm_code_selector = read_u16(shared, 28u);
  session->dosx_segment_to_selector = read_u32(shared, 30u);
  session->dosx_fault_handler_iret = read_u32(shared, 34u);
  session->dosx_fault_handler_iretd = read_u32(shared, 38u);
  session->dosx_int_handler_iret = read_u32(shared, 42u);
  session->dosx_int_handler_iretd = read_u32(shared, 46u);
  session->dosx_iret = read_u32(shared, 50u);
  session->dosx_iretd = read_u32(shared, 54u);
  session->initialized = 1u;
  return BX_NTVDM_DPMI_STARTUP_SESSION_OK;
}

uint32_t bx_ntvdm_dpmi_startup_session_initialize_app(
  struct bx_ntvdm_dpmi_startup_session *session,
  const bx_ntvdm_cpu_state_v1 *cpu_state, uint32_t current_dta_linear)
{
  if (!bx_ntvdm_dpmi_startup_session_valid(session) ||
      !bx_ntvdm_cpu_state_v1_valid(cpu_state) ||
      cpu_state->execution_mode != BX_NTVDM_CPU_EXECUTION_PROTECTED)
    return BX_NTVDM_DPMI_STARTUP_SESSION_REJECTED_CPU_MODE;
  /* DpmiInitApp keeps only bit zero of AX and captures the DTA/PSP values
   * supplied through its original protected stack frame.  The caller has
   * already copied that fixed-width frame; this seam never translates SS:SP. */
  session->current_app_flags = (uint16_t)(cpu_state->eax & 1u);
  session->current_dta_linear = current_dta_linear;
  return BX_NTVDM_DPMI_STARTUP_SESSION_OK;
}

uint32_t bx_ntvdm_dpmi_startup_session_publish_selector_table(
  struct bx_ntvdm_dpmi_startup_session *session, uint32_t selector_table_linear)
{
  if (!bx_ntvdm_dpmi_startup_session_valid(session) || selector_table_linear == 0u)
    return BX_NTVDM_DPMI_STARTUP_SESSION_REJECTED_INPUT;
  /* This is the original selGDT publication identity only.  Native Bochs
   * remains the descriptor-table owner; no LDT cache or host pointer exists. */
  session->selector_table_linear = selector_table_linear;
  return BX_NTVDM_DPMI_STARTUP_SESSION_OK;
}

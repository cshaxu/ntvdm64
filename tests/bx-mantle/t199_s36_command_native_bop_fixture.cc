#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"
#include <string.h>

static void profile_initialize(byob_profile_selection *profile)
{
  memset(profile, 0, sizeof(*profile));
  memcpy(profile->command_placement.path, L"\\COMMAND.COM",
    sizeof(L"\\COMMAND.COM"));
  profile->command_placement.drive_index = 2u;
  profile->has_command_placement = 1u;
  memcpy(profile->target_placement.path, L"\\TARGET.COM",
    sizeof(L"\\TARGET.COM"));
  profile->target_placement.drive_index = 2u;
  profile->has_target_placement = 1u;
  memcpy(profile->target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
  memcpy(profile->config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
  profile->config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
  memcpy(profile->autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
  profile->autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
  profile->has_guest_boot_files = 1u;
  profile->has_guest_search_metadata = 1u;
  profile->command_metadata.attributes = profile->target_metadata.attributes =
    profile->config_metadata.attributes = profile->autoexec_metadata.attributes = 0x20u;
  profile->command_metadata.dos_date = profile->target_metadata.dos_date =
    profile->config_metadata.dos_date = profile->autoexec_metadata.dos_date = 1u;
}

int main()
{
  uint8_t ntdos_bytes[] = { 0xfa, 0xfc, 0xf4 };
  uint8_t command_bytes[] = { 0x90, 0xc3 };
  uint8_t target_bytes[] = { 0xf4 };
  byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
  byob_image command = { command_bytes, sizeof(command_bytes) };
  byob_image target = { target_bytes, sizeof(target_bytes) };
  byob_profile_selection profile;
  bx_ntvdm_boot_namespace_composition_v1 composition;
  bx_ntvdm_finite_run_request request;
  bx_ntvdm_finite_run_status status;

  profile_initialize(&profile);
  if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
      &ntdos, &command, &target, 0, &profile) ||
      !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;

  memset(&request, 0, sizeof(request));
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  request.entry_bytes[0] = 0xc4u;
  request.entry_bytes[1] = 0xc4u;
  request.entry_bytes[2] = 0x54u;
  request.entry_bytes[3] = 0x00u;
  request.entry_byte_count = 4u;
  request.entry_physical_address = 0u;
  request.entry_cs = 0u;
  request.entry_eip = 0u;
  request.instruction_tick_budget = 64u;
  request.ips = 1000000u;
  request.stop_on_ud_fixture = 0;
  status = bx_ntvdm_run_finite_bare_bytes(&request);
  bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
  return status == BX_NTVDM_FINITE_RUN_COMPLETED_UD_STOP ? 0 :
    20 + (int)status;
}

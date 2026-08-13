#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

#include <string.h>

#ifndef BX_NTVDM_NATIVE_BOOT_SERVICE
#define BX_NTVDM_NATIVE_BOOT_SERVICE 0x0c
#endif

int main()
{
  /* `C4 C4` is the historical BOP #UD form.  The following HLT gives the
   * already finite native runner its existing selector-blind terminal path
   * after the adapter returns the original typed RESUME. */
  static const Bit8u bytes[] = { 0xc4, 0xc4, 0x54, BX_NTVDM_NATIVE_BOOT_SERVICE, 0xf4 };
  static uint8_t command_bytes[] = { 0x90, 0xc3 };
  static uint8_t target_bytes[] = { 0xf4 };
  byob_image command = { command_bytes, sizeof(command_bytes) };
  byob_image target = { target_bytes, sizeof(target_bytes) };
  byob_profile_selection profile;
  bx_ntvdm_boot_namespace_composition_v1 composition;
  bx_ntvdm_finite_run_request request;
  int status;

  memset(&profile, 0, sizeof(profile));
  memcpy(profile.command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
  profile.command_placement.drive_index = 2;
  profile.has_command_placement = 1;
  memcpy(profile.target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
  profile.target_placement.drive_index = 2;
  profile.has_target_placement = 1;
  memcpy(profile.target.file_name, L"TARGET.COM", sizeof(L"TARGET.COM"));
  memcpy(profile.config_file.path, L"\\CONFIG.SYS", sizeof(L"\\CONFIG.SYS"));
  profile.config_file.materialization = BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
  memcpy(profile.autoexec_file.path, L"\\AUTOEXEC.BAT", sizeof(L"\\AUTOEXEC.BAT"));
  profile.autoexec_file.materialization = BYOB_GUEST_BOOT_FILE_EMPTY_V1;
  profile.has_guest_boot_files = profile.has_guest_search_metadata = 1;
  profile.command_metadata.attributes = profile.target_metadata.attributes =
    profile.config_metadata.attributes = profile.autoexec_metadata.attributes = 0x20;
  profile.command_metadata.dos_date = profile.target_metadata.dos_date =
    profile.config_metadata.dos_date = profile.autoexec_metadata.dos_date = 1;
  if (!bx_ntvdm_boot_namespace_composition_v1_initialize(&composition,
      0, &command, &target, 0, &profile) ||
      !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;
  request.entry_bytes = bytes;
  request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000;
  request.entry_cs = 0x0100;
  request.entry_eip = 0;
  request.instruction_tick_budget = 64;
  request.ips = 1000000;
  request.stop_on_ud_fixture = 0;
  status = (int) bx_ntvdm_run_finite_bare_bytes(&request);
  bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
  return status;
}

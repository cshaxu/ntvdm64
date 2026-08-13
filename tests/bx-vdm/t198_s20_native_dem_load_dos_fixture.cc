#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

#include <string.h>

int main()
{
  /* The copied NTDOS test image starts with HLT.  After the real 50:11 BOP
   * resumes, the guest far-jumps to DI:0000.  Reaching that HLT proves the
   * complete checked RAM publication, rather than only adapter acceptance. */
  static Bit8u ntdos_bytes[0x6cd2];
  static Bit8u bytes[] = {
    0xbf,0xb0,0x08,             /* mov di,08b0 */
    0xc4,0xc4,0x50,0x11,        /* DEM load-DOS */
    0xea,0x00,0x00,0xb0,0x08    /* jmp 08b0:0000 */
  };
  static uint8_t command_bytes[] = { 0x90, 0xc3 };
  static uint8_t target_bytes[] = { 0xf4 };
  byob_image ntdos = { ntdos_bytes, sizeof(ntdos_bytes) };
  byob_image command = { command_bytes, sizeof(command_bytes) };
  byob_image target = { target_bytes, sizeof(target_bytes) };
  byob_profile_selection profile;
  bx_ntvdm_boot_namespace_composition_v1 composition;
  static bx_ntvdm_finite_run_request request;
  unsigned status;

  memset(&profile, 0, sizeof(profile));
  ntdos_bytes[0] = 0xf4;
  profile.ntdos.bytes = sizeof(ntdos_bytes);
  memcpy(profile.ntdos.file_name, L"NTDOS.SYS", sizeof(L"NTDOS.SYS"));
  memcpy(profile.command_placement.path, L"\\COMMAND.COM", sizeof(L"\\COMMAND.COM"));
  profile.command_placement.drive_index = 2; profile.has_command_placement = 1;
  memcpy(profile.target_placement.path, L"\\TARGET.COM", sizeof(L"\\TARGET.COM"));
  profile.target_placement.drive_index = 2; profile.has_target_placement = 1;
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
      &ntdos, &command, &target, 0, &profile) ||
      !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, bytes, sizeof(bytes)); request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000; request.entry_cs = 0x0100;
  request.entry_eip = 0; request.instruction_tick_budget = 64;
  request.ips = 1000000; request.stop_on_ud_fixture = 0;
  request.preserve_physical_address = 0; request.preserve_byte_count = 0;
  status = (unsigned)bx_ntvdm_run_finite_bare_bytes(&request);
  bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
  return (int)status;
}

#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

#include <string.h>

int main()
{
  /* This is one real-mode payload at 0100:0000.  It registers the OpenNT
   * DEM DTA locations, performs PATHFIRST and then PATHNEXT.  All pointers
   * and the search result DTA are ordinary guest RAM; the fixture supplies
   * neither a host register result nor a direct plane call. */
  static Bit8u bytes[0x190];
  static uint8_t command_bytes[] = { 0x90, 0xc3 };
  static uint8_t target_bytes[] = { 0xf4 };
  static const Bit8u code[] = {
    /* DS=0100; AX/DX/CX/SI name the original demSetDTALocation inputs. */
    0xb8,0x00,0x01, 0x8e,0xd8,
    0xb8,0x80,0x00, 0xba,0x90,0x00, 0xb9,0xa0,0x00, 0xbe,0x20,0x00,
    0xc4,0xc4,0x50,0x1b,
    /* PATHFIRST uses DS:DX pathname and CX attribute mask. */
    0xba,0x00,0x01, 0x31,0xc9, 0xc4,0xc4,0x50,0x09,
    /* PATHNEXT consumes the DTA image written by PATHFIRST. */
    0xc4,0xc4,0x50,0x0b, 0xf4
  };
  byob_image command = { command_bytes, sizeof(command_bytes) };
  byob_image target = { target_bytes, sizeof(target_bytes) };
  byob_profile_selection profile;
  bx_ntvdm_boot_namespace_composition_v1 composition;
  bx_ntvdm_finite_run_request request;
  unsigned status;

  memset(bytes, 0, sizeof(bytes));
  memcpy(bytes, code, sizeof(code));
  /* DOSWOWDATA::lpSftAddr at DS:SI+32: offset 0050 in the same segment. */
  bytes[0x40] = 0x50;
  /* DS:0080 is the DTA far-pointer variable, not the DTA itself. */
  bytes[0x80] = 0xc0; bytes[0x81] = 0x00;
  bytes[0x82] = 0x00; bytes[0x83] = 0x01;
  /* DS:0090 is the current-PDB word.  The DTA itself starts at DS:00c0. */
  bytes[0x90] = 0x34; bytes[0x91] = 0x12;
  memcpy(bytes + 0x100, "C:\\*.*", sizeof("C:\\*.*"));

  memset(&profile, 0, sizeof(profile));
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
      &command, &target, 0, &profile) ||
      !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;
  request.entry_bytes = bytes; request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000; request.entry_cs = 0x0100;
  request.entry_eip = 0; request.instruction_tick_budget = 128;
  request.ips = 1000000; request.stop_on_ud_fixture = 0;
  status = (unsigned)bx_ntvdm_run_finite_bare_bytes(&request);
  bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
  return (int)status;
}

#include "bochs.h"
#include "adapter-softpc/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"

#include <string.h>

int main()
{
  /* The opaque payload is real-mode code at 1000:0000 plus pathname bytes at
   * 0100:0100.  The guest, not the fixture, carries AX:BP token state between
   * the existing 50:12, 50:16, 50:00 and 50:02 services. */
  static Bit8u bytes[0x110];
  static uint8_t command_bytes[] = { 0x90, 0xc3 };
  static uint8_t target_bytes[] = { 0xf4 };
  byob_image command = { command_bytes, sizeof(command_bytes) };
  byob_image target = { target_bytes, sizeof(target_bytes) };
  byob_profile_selection profile;
  bx_ntvdm_boot_namespace_composition_v1 composition;
  static bx_ntvdm_finite_run_request request;
  unsigned i = 0;

  memset(bytes, 0, sizeof(bytes));
  /* mov ax,0100; mov ds,ax; xor ax,ax; mov si,0100; BOP 50:12 */
  static const Bit8u code[] = {
    0xb8,0x00,0x01, 0x8e,0xd8, 0x31,0xc0, 0xbe,0x00,0x01,
    0xc4,0xc4,0x50,0x12,
    /* save token AX in DI; ZF=1, count=2, destination=0100:0200 */
    0x89,0xc7, 0x31,0xdb, 0xb9,0x02,0x00, 0xba,0x00,0x02,
    0xbe,0x00,0x00, 0xc4,0xc4,0x50,0x16,
    /* restore token; seek absolute zero; restore token; close; HLT */
    0x89,0xf8, 0x31,0xdb, 0x31,0xc9, 0x31,0xd2,
    0xc4,0xc4,0x50,0x00, 0x89,0xf8, 0xc4,0xc4,0x50,0x02, 0xf4
  };
  memcpy(bytes, code, sizeof(code));
  memcpy(bytes + 0x100, "C:\\CONFIG.SYS", sizeof("C:\\CONFIG.SYS"));

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
      0, &command, &target, 0, &profile) ||
      !bx_ntvdm_boot_namespace_composition_v1_bind(&composition)) return 1;
  request.request_version = BX_NTVDM_FINITE_RUN_REQUEST_VERSION;
  memcpy(request.entry_bytes, bytes, sizeof(bytes)); request.entry_byte_count = sizeof(bytes);
  request.entry_physical_address = 0x1000; request.entry_cs = 0x0100;
  request.entry_eip = 0; request.instruction_tick_budget = 128;
  request.ips = 1000000;
  request.preserve_physical_address = 0; request.preserve_byte_count = 0;
  i = (unsigned)bx_ntvdm_run_finite_bare_bytes(&request);
  bx_ntvdm_boot_namespace_composition_v1_unbind(&composition);
  return (int)i;
}

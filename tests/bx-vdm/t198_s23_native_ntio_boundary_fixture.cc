#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-vdm/bx_ntvdm_ntio_preentry_v1.h"
#include <stdio.h>
#include <string.h>

extern const Bit8u t198_s23_ntio_bytes[0x8400];
extern const Bit8u t198_s25_ntdos_bytes[0x6cd2];
extern "C" unsigned t198_s23_native_ntio_boundary_observed_5011(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_503b_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_dta_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_stop(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_selector(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_service(void);

int main()
{
  static uint8_t command[] = {0x90,0xc3};
  static uint8_t target[] = {0xf4}; byob_image ntio={ (uint8_t*)t198_s23_ntio_bytes,0x8400};
  byob_image ntdos={(uint8_t*)t198_s25_ntdos_bytes,0x6cd2}, cmd={command,sizeof(command)}, tgt={target,sizeof(target)};
  byob_profile_selection p; bx_ntvdm_boot_namespace_composition_v1 c;
  static bx_ntvdm_finite_run_request r; bx_ntvdm_finite_run_terminal_snapshot terminal; unsigned terminal_valid; int status;
  memset(&p,0,sizeof(p)); p.ntio.bytes=0x8400; p.ntdos.bytes=0x6cd2;
  memcpy(p.command_placement.path,L"\\COMMAND.COM",sizeof(L"\\COMMAND.COM"));p.command_placement.drive_index=2;p.has_command_placement=1;
  memcpy(p.target_placement.path,L"\\TARGET.COM",sizeof(L"\\TARGET.COM"));p.target_placement.drive_index=2;p.has_target_placement=1;
  memcpy(p.target.file_name,L"TARGET.COM",sizeof(L"TARGET.COM"));p.has_guest_boot_files=p.has_guest_search_metadata=1;
  memcpy(p.config_file.path,L"\\CONFIG.SYS",sizeof(L"\\CONFIG.SYS"));p.config_file.materialization=BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
  memcpy(p.autoexec_file.path,L"\\AUTOEXEC.BAT",sizeof(L"\\AUTOEXEC.BAT"));p.autoexec_file.materialization=BYOB_GUEST_BOOT_FILE_EMPTY_V1;
  p.command_metadata.attributes=p.target_metadata.attributes=p.config_metadata.attributes=p.autoexec_metadata.attributes=0x20;
  p.command_metadata.dos_date=p.target_metadata.dos_date=p.config_metadata.dos_date=p.autoexec_metadata.dos_date=1;
  if(!bx_ntvdm_boot_namespace_composition_v1_initialize(&c,&ntdos,&cmd,&tgt,0,&p)||!bx_ntvdm_boot_namespace_composition_v1_bind(&c)||!bx_ntvdm_ntio_preentry_v1_prepare(&ntio,&p.ntio,&r,8192,1000000))return 1;
  memset(&terminal,0,sizeof(terminal)); r.capture_terminal_snapshot=1; status=(int)bx_ntvdm_run_finite_bare_bytes(&r); terminal_valid=bx_ntvdm_finite_run_terminal_snapshot_get(&terminal); bx_ntvdm_boot_namespace_composition_v1_unbind(&c);
  fprintf(stderr,"t198-s23 status=%d observed-5011=%u observed-503b-resume=%u observed-dta-resume=%u observed-stop=%u next=%02x:%02x plane=%u/%u/%u terminal=%u:%04x:%04x\n",status,
    t198_s23_native_ntio_boundary_observed_5011(),t198_s23_native_ntio_boundary_observed_503b_resume(),t198_s23_native_ntio_boundary_observed_dta_resume(),t198_s23_native_ntio_boundary_observed_stop(),
    t198_s23_native_ntio_boundary_observed_selector(),t198_s23_native_ntio_boundary_observed_service(),c.plane.pending_kind,c.plane.pending_action_id,c.plane.has_dta,
    terminal_valid,terminal.cs,terminal.eip);
  return t198_s23_native_ntio_boundary_observed_5011() &&
    t198_s23_native_ntio_boundary_observed_503b_resume() &&
    t198_s23_native_ntio_boundary_observed_dta_resume() &&
    t198_s23_native_ntio_boundary_observed_stop()?0:2;
}

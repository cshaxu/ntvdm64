#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-mantle/bx_ntvdm_instruction_history.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-vdm/bx_ntvdm_ntio_preentry_v1.h"
#include <stdio.h>
#include <string.h>

extern const Bit8u t198_s23_ntio_bytes[0x8400];
extern const Bit8u t198_s25_ntdos_bytes[0x6cd2];
extern "C" unsigned t198_s23_native_ntio_boundary_observed_5011(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_503b_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_dta_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_hard_error_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_system_symbol_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_drive_resume(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_ud(void);
extern "C" int t198_s23_native_ntio_boundary_copy_first_generic_event(struct bx_ntvdm_generic_ud_event_v1 *value);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_mode(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_vector(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_window_bytes(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_window(unsigned index);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_history_count(void);
extern "C" int t198_s23_native_ntio_boundary_copy_first_generic_history(unsigned index, struct bx_ntvdm_instruction_history_record_v1 *value);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_stop(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_selector(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_service(void);

int main()
{
  static uint8_t command[] = {0x90,0xc3};
  static uint8_t target[] = {0xf4}; byob_image ntio={ (uint8_t*)t198_s23_ntio_bytes,0x8400};
  byob_image ntdos={(uint8_t*)t198_s25_ntdos_bytes,0x6cd2}, cmd={command,sizeof(command)}, tgt={target,sizeof(target)};
  byob_profile_selection p; bx_ntvdm_boot_namespace_composition_v1 c; bx_ntvdm_host_drive_snapshot_v1 drives; uint8_t drive_types[26]={0};
  static bx_ntvdm_finite_run_request r; bx_ntvdm_finite_run_terminal_snapshot terminal; bx_ntvdm_generic_ud_event_v1 generic; bx_ntvdm_instruction_history_record_v1 history[8]; unsigned terminal_valid, generic_valid, history_count; int status;
  memset(&p,0,sizeof(p)); p.ntio.bytes=0x8400; p.ntdos.bytes=0x6cd2;
  memcpy(p.command_placement.path,L"\\COMMAND.COM",sizeof(L"\\COMMAND.COM"));p.command_placement.drive_index=2;p.has_command_placement=1;
  memcpy(p.target_placement.path,L"\\TARGET.COM",sizeof(L"\\TARGET.COM"));p.target_placement.drive_index=2;p.has_target_placement=1;
  memcpy(p.target.file_name,L"TARGET.COM",sizeof(L"TARGET.COM"));p.has_guest_boot_files=p.has_guest_search_metadata=1;
  memcpy(p.config_file.path,L"\\CONFIG.SYS",sizeof(L"\\CONFIG.SYS"));p.config_file.materialization=BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
  memcpy(p.autoexec_file.path,L"\\AUTOEXEC.BAT",sizeof(L"\\AUTOEXEC.BAT"));p.autoexec_file.materialization=BYOB_GUEST_BOOT_FILE_EMPTY_V1;
  p.command_metadata.attributes=p.target_metadata.attributes=p.config_metadata.attributes=p.autoexec_metadata.attributes=0x20;
  p.command_metadata.dos_date=p.target_metadata.dos_date=p.config_metadata.dos_date=p.autoexec_metadata.dos_date=1;
  drive_types[2]=3u;
  if(!bx_ntvdm_host_drive_snapshot_v1_apply(1u<<2,drive_types,0u,0u,&drives)||!bx_ntvdm_boot_namespace_composition_v1_initialize(&c,&ntdos,&cmd,&tgt,0,&p)||!bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&c,&drives)||!bx_ntvdm_boot_namespace_composition_v1_bind(&c)||!bx_ntvdm_ntio_preentry_v1_prepare(&ntio,&p.ntio,&r,8192,1000000))return 1;
  if(!bx_ntvdm_mantle_instruction_history_v1_configure(0u)||bx_ntvdm_mantle_instruction_history_v1_count()!=0u||bx_ntvdm_mantle_instruction_history_v1_configure(BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX+1u)||!bx_ntvdm_mantle_instruction_history_v1_configure(8u))return 1;
  if(!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0u,0u)||bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0x100000u,1u)||!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0x1370u,32u))return 1;
  memset(&terminal,0,sizeof(terminal)); memset(&generic,0,sizeof(generic)); memset(history,0,sizeof(history)); r.capture_terminal_snapshot=1; status=(int)bx_ntvdm_run_finite_bare_bytes(&r); terminal_valid=bx_ntvdm_finite_run_terminal_snapshot_get(&terminal); generic_valid=t198_s23_native_ntio_boundary_copy_first_generic_event(&generic); history_count=t198_s23_native_ntio_boundary_observed_first_generic_history_count(); for(unsigned n=0;n<history_count;n++)if(!t198_s23_native_ntio_boundary_copy_first_generic_history(n,&history[n]))return 1; bx_ntvdm_boot_namespace_composition_v1_unbind(&c);
  fprintf(stderr,"t198-s23 status=%d observed-5011=%u observed-503b-resume=%u observed-dta-resume=%u observed-hard-error-resume=%u observed-system-symbol-resume=%u observed-drive-resume=%u observed-first-generic-ud=%u fault=%04x:%04x mode=%u vector=%u window=%u:%02x%02x%02x%02x state=%u:%04x:%04x ss-sp=%04x:%04x ds-si=%04x:%04x es-di=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ebp=%08x flags=%08x observed-stop=%u next=%02x:%02x plane=%u/%u/%u error-lock=%u gset=%u terminal=%u:%04x:%04x snapshot=%u:%05x:%u stack=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x history=%u\n",status,
    t198_s23_native_ntio_boundary_observed_5011(),t198_s23_native_ntio_boundary_observed_503b_resume(),t198_s23_native_ntio_boundary_observed_dta_resume(),t198_s23_native_ntio_boundary_observed_hard_error_resume(),t198_s23_native_ntio_boundary_observed_system_symbol_resume(),t198_s23_native_ntio_boundary_observed_drive_resume(),t198_s23_native_ntio_boundary_observed_first_generic_ud(),t198_s23_native_ntio_boundary_observed_first_generic_cs(),t198_s23_native_ntio_boundary_observed_first_generic_eip(),t198_s23_native_ntio_boundary_observed_first_generic_mode(),t198_s23_native_ntio_boundary_observed_first_generic_vector(),t198_s23_native_ntio_boundary_observed_first_generic_window_bytes(),t198_s23_native_ntio_boundary_observed_first_generic_window(0),t198_s23_native_ntio_boundary_observed_first_generic_window(1),t198_s23_native_ntio_boundary_observed_first_generic_window(2),t198_s23_native_ntio_boundary_observed_first_generic_window(3),generic_valid,(unsigned)generic.cs,(unsigned)generic.eip,(unsigned)generic.ss,(unsigned)generic.esp,(unsigned)generic.ds,(unsigned)generic.esi,(unsigned)generic.es,(unsigned)generic.edi,(unsigned)generic.eax,(unsigned)generic.ebx,(unsigned)generic.ecx,(unsigned)generic.edx,(unsigned)generic.ebp,(unsigned)generic.eflags,t198_s23_native_ntio_boundary_observed_stop(),
    t198_s23_native_ntio_boundary_observed_selector(),t198_s23_native_ntio_boundary_observed_service(),c.plane.pending_kind,c.plane.pending_action_id,c.plane.has_dta,c.error_lock.has_registration,c.gset.has_drive_snapshot,
    terminal_valid,terminal.cs,terminal.eip,terminal.valid,(unsigned)terminal.captured_physical_address,terminal.captured_byte_count,terminal.captured_bytes[10],terminal.captured_bytes[11],terminal.captured_bytes[12],terminal.captured_bytes[13],terminal.captured_bytes[14],terminal.captured_bytes[15],terminal.captured_bytes[16],terminal.captured_bytes[17],terminal.captured_bytes[18],terminal.captured_bytes[19],terminal.captured_bytes[20],terminal.captured_bytes[21],terminal.captured_bytes[22],terminal.captured_bytes[23],terminal.captured_bytes[24],terminal.captured_bytes[25],history_count);
  fprintf(stderr,"t198-s23 history-tail count=%u predecessor=%04x:%04x seq=%llx fault=%04x:%04x seq=%llx\n",history_count,history[6].cs,(unsigned)history[6].rip,(unsigned long long)history[6].sequence,history[7].cs,(unsigned)history[7].rip,(unsigned long long)history[7].sequence);
  return t198_s23_native_ntio_boundary_observed_first_generic_ud() && generic_valid &&
    generic.magic==BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC && generic.abi_version==BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION && generic.struct_bytes==sizeof(generic) && generic.cs==0u && generic.eip==0x0a84u && generic.execution_mode==1u && generic.vector==6u && generic.window_bytes>=4u && generic.window[0]==0xffu && generic.window[1]==0xffu && generic.window[2]==0xffu && generic.window[3]==0xffu &&
    t198_s23_native_ntio_boundary_observed_stop() && terminal_valid &&
    terminal.captured_physical_address==0x1370u && terminal.captured_byte_count==32u && history_count==8u &&
    history[7].version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && history[7].cs==0u && history[7].rip==0x0a84u &&
    history[6].version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && history[6].sequence+1u==history[7].sequence?0:2;
}

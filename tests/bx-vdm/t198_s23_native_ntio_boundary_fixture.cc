#include "bochs.h"
#include "bx-mantle/bx_ntvdm_finite_run.h"
#include "bx-mantle/bx_ntvdm_instruction_history.h"
#include "bx-vdm/bx_ntvdm_boot_namespace_composition_v1.h"
#include "bx-vdm/bx_ntvdm_ntio_preentry_v1.h"
#include "t198_s23_fastread_attempt_ledger.h"
#include <stdio.h>
#include <string.h>

extern const Bit8u t198_s23_ntio_bytes[0x8400];
extern const Bit8u t198_s25_ntdos_bytes[0x6cd2];
#ifdef T198_S93_SOURCE_BUILT_NORMAL_RETURN
/* Compile-only S93 input seam: these arrays are generated from the locked
 * source-built artifacts.  This fixture still does not execute under S93. */
extern const Bit8u t198_s93_command_bytes[0xc4d0];
extern const Bit8u t198_s93_share_bytes[0x372];
#endif
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
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_generic_cs_transition_valid(void);
extern "C" int t198_s23_native_ntio_boundary_copy_first_generic_cs_transition(struct bx_ntvdm_instruction_history_transition_v1 *value);
extern "C" int t198_s23_native_ntio_boundary_copy_first_generic_cs_provenance(struct bx_ntvdm_instruction_history_provenance_v1 *value);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_canonical_bop(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_canonical_selector(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_canonical_service(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_canonical_host_int10_read(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_first_canonical_host_int10(unsigned index);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_stop(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_selector(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_service(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_next_byte(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_composition_handled(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_bios15_outcome(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_ioctl_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_es(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_esi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_edi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_ebp(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_commit(void);
extern "C" unsigned long long t198_s23_native_ntio_boundary_observed_fast_read_resume_rip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_gpr16_write_mask(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_ax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_eflags_write_mask(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_fast_read_eflags_values(void);
extern "C" unsigned t198_s23_native_ntio_boundary_fast_read_attempt_count(void);
extern "C" int t198_s23_native_ntio_boundary_copy_fast_read_attempt(unsigned index, struct t198_s23_fastread_attempt_v1 *entry);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_es(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_esi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_edi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_spckbd_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_emm_probe_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_mouse_install1_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_printer_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_config_done_eflags(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_cs(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_ds(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_eip(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_eax(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_ebx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_ecx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_edx(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_esi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_edi(void);
extern "C" unsigned t198_s23_native_ntio_boundary_observed_entry_dos_app_eflags(void);

static int prepare_preentry_input(bx_ntvdm_preentry_input_v1 *input)
{
  static const uint8_t ivt[] = {
    0x4a,0x01,0x00,0xc0,0x4d,0xf8,0x00,0xf0,0x41,0xf8,0x00,0xf0,
    0xfe,0xe3,0x00,0xf0,0x39,0xe7,0x00,0xf0,0x59,0xf8,0x00,0xf0,
    0x2e,0xe8,0x00,0xf0,0xd2,0xef,0x00,0xf0,0x95,0x96,0x00,0xf0,
    0xf2,0xe6,0x00,0xf0,0x6e,0xfe,0x00,0xf0,0x53,0xff,0x00,0xf0 };
  bx_ntvdm_preentry_input_v1_clear(input);
  input->range_count=3u; input->payload_bytes=53u;
  input->ranges[0].physical_address=0x40u; input->ranges[0].byte_count=48u;
  input->ranges[1].physical_address=0x496u; input->ranges[1].byte_count=1u; input->ranges[1].payload_offset=48u;
  input->ranges[2].physical_address=0x714u; input->ranges[2].byte_count=4u; input->ranges[2].payload_offset=49u;
  memcpy(input->payload,ivt,sizeof(ivt)); input->payload[48]=0x10u;
  input->payload[49]=0u; input->payload[50]=0u; input->payload[51]=0u; input->payload[52]=0xc0u;
  return bx_ntvdm_preentry_input_v1_valid(input);
}

int main()
{
#ifdef T198_S93_SOURCE_BUILT_NORMAL_RETURN
  byob_image ntio={ (uint8_t*)t198_s23_ntio_bytes,0x8400};
  byob_image ntdos={(uint8_t*)t198_s25_ntdos_bytes,0x6cd2}, cmd={(uint8_t*)t198_s93_command_bytes,0xc4d0}, tgt={(uint8_t*)t198_s93_share_bytes,0x372};
#else
  static uint8_t command[] = {0x90,0xc3};
  static uint8_t target[] = {0xf4}; byob_image ntio={ (uint8_t*)t198_s23_ntio_bytes,0x8400};
  byob_image ntdos={(uint8_t*)t198_s25_ntdos_bytes,0x6cd2}, cmd={command,sizeof(command)}, tgt={target,sizeof(target)};
#endif
  byob_profile_selection p; bx_ntvdm_boot_namespace_composition_v1 c; bx_ntvdm_host_drive_snapshot_v1 drives; bx_ntvdm_preentry_input_v1 preentry; uint8_t drive_types[26]={0};
  static bx_ntvdm_finite_run_request r; bx_ntvdm_finite_run_terminal_snapshot terminal; bx_ntvdm_generic_ud_event_v1 generic; bx_ntvdm_instruction_history_record_v1 history[8]; bx_ntvdm_instruction_history_transition_v1 transition; bx_ntvdm_instruction_history_provenance_v1 provenance; unsigned terminal_valid, generic_valid, history_count, transition_valid, provenance_valid; int status;
  memset(&p,0,sizeof(p)); p.ntio.bytes=0x8400; p.ntdos.bytes=0x6cd2;
  /* This fixture supplies the already parser-validated v7 projection.  It
   * does not parse or invent a display option inside the native machine. */
  p.guest_display_state=BYOB_GUEST_DISPLAY_STATE_STREAM_IO_V1;
  memcpy(p.command_placement.path,L"\\COMMAND.COM",sizeof(L"\\COMMAND.COM"));p.command_placement.drive_index=2;p.has_command_placement=1;
#ifdef T198_S93_SOURCE_BUILT_NORMAL_RETURN
  memcpy(p.target_placement.path,L"\\TARGET.EXE",sizeof(L"\\TARGET.EXE"));p.target_placement.drive_index=2;p.has_target_placement=1;
  memcpy(p.target.file_name,L"TARGET.EXE",sizeof(L"TARGET.EXE"));
#else
  memcpy(p.target_placement.path,L"\\TARGET.COM",sizeof(L"\\TARGET.COM"));p.target_placement.drive_index=2;p.has_target_placement=1;
  memcpy(p.target.file_name,L"TARGET.COM",sizeof(L"TARGET.COM"));
#endif
  p.has_guest_boot_files=p.has_guest_search_metadata=1;
  memcpy(p.config_file.path,L"\\CONFIG.SYS",sizeof(L"\\CONFIG.SYS"));p.config_file.materialization=BYOB_GUEST_BOOT_FILE_MINIMAL_COMMENT_V1;
  memcpy(p.autoexec_file.path,L"\\AUTOEXEC.BAT",sizeof(L"\\AUTOEXEC.BAT"));p.autoexec_file.materialization=BYOB_GUEST_BOOT_FILE_EMPTY_V1;
  p.command_metadata.attributes=p.target_metadata.attributes=p.config_metadata.attributes=p.autoexec_metadata.attributes=0x20;
  p.command_metadata.dos_date=p.target_metadata.dos_date=p.config_metadata.dos_date=p.autoexec_metadata.dos_date=1;
  drive_types[2]=3u;
  if(!prepare_preentry_input(&preentry)||!bx_ntvdm_host_drive_snapshot_v1_apply(1u<<2,drive_types,0u,0u,&drives)||!bx_ntvdm_boot_namespace_composition_v1_initialize(&c,&ntdos,&cmd,&tgt,0,&p)||!bx_ntvdm_boot_namespace_composition_v1_set_drive_snapshot(&c,&drives)||!bx_ntvdm_boot_namespace_composition_v1_bind(&c)||!bx_ntvdm_ntio_preentry_v1_prepare(&ntio,&p.ntio,&preentry,&r,1000000,1000000))return 1;
  if(!bx_ntvdm_mantle_instruction_history_v1_configure(0u)||bx_ntvdm_mantle_instruction_history_v1_count()!=0u||bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_transition(&transition)||bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_provenance(&provenance)||bx_ntvdm_mantle_instruction_history_v1_configure(BX_NTVDM_INSTRUCTION_HISTORY_V1_CAPACITY_MAX+1u)||!bx_ntvdm_mantle_instruction_history_v1_configure(8u)||bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_transition(&transition)||bx_ntvdm_mantle_instruction_history_v1_get_latest_cs_provenance(&provenance))return 1;
  if(!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0u,0u)||bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0x100000u,1u)||!bx_ntvdm_finite_run_terminal_snapshot_configure_ordinary_range(0x0deau,4u))return 1;
  memset(&terminal,0,sizeof(terminal)); memset(&generic,0,sizeof(generic)); memset(history,0,sizeof(history)); memset(&transition,0,sizeof(transition)); memset(&provenance,0,sizeof(provenance)); r.capture_terminal_snapshot=1; status=(int)bx_ntvdm_run_finite_bare_bytes(&r); terminal_valid=bx_ntvdm_finite_run_terminal_snapshot_get(&terminal); generic_valid=t198_s23_native_ntio_boundary_copy_first_generic_event(&generic); history_count=t198_s23_native_ntio_boundary_observed_first_generic_history_count(); for(unsigned n=0;n<history_count;n++)if(!t198_s23_native_ntio_boundary_copy_first_generic_history(n,&history[n]))return 1; transition_valid=t198_s23_native_ntio_boundary_copy_first_generic_cs_transition(&transition); provenance_valid=t198_s23_native_ntio_boundary_copy_first_generic_cs_provenance(&provenance); bx_ntvdm_boot_namespace_composition_v1_unbind(&c);
  fprintf(stderr,"t198-s23 status=%d observed-5011=%u observed-503b-resume=%u observed-dta-resume=%u observed-hard-error-resume=%u observed-system-symbol-resume=%u observed-drive-resume=%u observed-first-generic-ud=%u fault=%04x:%04x mode=%u vector=%u window=%u:%02x%02x%02x%02x state=%u:%04x:%04x ss-sp=%04x:%04x ds-si=%04x:%04x es-di=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ebp=%08x flags=%08x observed-stop=%u next=%02x:%02x plane=%u/%u/%u error-lock=%u gset=%u terminal=%u:%04x:%04x snapshot=%u:%05x:%u call-dev-ad=%02x%02x%02x%02x history=%u\n",status,
    t198_s23_native_ntio_boundary_observed_5011(),t198_s23_native_ntio_boundary_observed_503b_resume(),t198_s23_native_ntio_boundary_observed_dta_resume(),t198_s23_native_ntio_boundary_observed_hard_error_resume(),t198_s23_native_ntio_boundary_observed_system_symbol_resume(),t198_s23_native_ntio_boundary_observed_drive_resume(),t198_s23_native_ntio_boundary_observed_first_generic_ud(),t198_s23_native_ntio_boundary_observed_first_generic_cs(),t198_s23_native_ntio_boundary_observed_first_generic_eip(),t198_s23_native_ntio_boundary_observed_first_generic_mode(),t198_s23_native_ntio_boundary_observed_first_generic_vector(),t198_s23_native_ntio_boundary_observed_first_generic_window_bytes(),t198_s23_native_ntio_boundary_observed_first_generic_window(0),t198_s23_native_ntio_boundary_observed_first_generic_window(1),t198_s23_native_ntio_boundary_observed_first_generic_window(2),t198_s23_native_ntio_boundary_observed_first_generic_window(3),generic_valid,(unsigned)generic.cs,(unsigned)generic.eip,(unsigned)generic.ss,(unsigned)generic.esp,(unsigned)generic.ds,(unsigned)generic.esi,(unsigned)generic.es,(unsigned)generic.edi,(unsigned)generic.eax,(unsigned)generic.ebx,(unsigned)generic.ecx,(unsigned)generic.edx,(unsigned)generic.ebp,(unsigned)generic.eflags,t198_s23_native_ntio_boundary_observed_stop(),
    t198_s23_native_ntio_boundary_observed_selector(),t198_s23_native_ntio_boundary_observed_service(),c.plane.pending_kind,c.plane.pending_action_id,c.plane.has_dta,c.error_lock.has_registration,c.gset.has_drive_snapshot,
    terminal_valid,terminal.cs,terminal.eip,terminal.valid,(unsigned)terminal.captured_physical_address,terminal.captured_byte_count,terminal.captured_bytes[0],terminal.captured_bytes[1],terminal.captured_bytes[2],terminal.captured_bytes[3],history_count);
  fprintf(stderr,"t198-s23 history-tail count=%u predecessor=%04x:%04x seq=%llx fault=%04x:%04x seq=%llx\n",history_count,history[6].cs,(unsigned)history[6].rip,(unsigned long long)history[6].sequence,history[7].cs,(unsigned)history[7].rip,(unsigned long long)history[7].sequence);
  fprintf(stderr,"t198-s23 cs-transition valid=%u previous=%04x:%04x ss-sp=%04x:%04x seq=%llx current=%04x:%04x ss-sp=%04x:%04x seq=%llx\n",transition_valid,transition.previous.cs,(unsigned)transition.previous.rip,transition.previous.ss,transition.previous.sp,(unsigned long long)transition.previous.sequence,transition.current.cs,(unsigned)transition.current.rip,transition.current.ss,transition.current.sp,(unsigned long long)transition.current.sequence);
  fprintf(stderr,"t198-s23 cs-provenance valid=%u predecessor-valid=%u stack-valid=%u code=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x stack=%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",provenance_valid,provenance.predecessor_valid,provenance.stack_valid,provenance.predecessor_bytes[0],provenance.predecessor_bytes[1],provenance.predecessor_bytes[2],provenance.predecessor_bytes[3],provenance.predecessor_bytes[4],provenance.predecessor_bytes[5],provenance.predecessor_bytes[6],provenance.predecessor_bytes[7],provenance.predecessor_bytes[8],provenance.predecessor_bytes[9],provenance.predecessor_bytes[10],provenance.predecessor_bytes[11],provenance.predecessor_bytes[12],provenance.predecessor_bytes[13],provenance.predecessor_bytes[14],provenance.stack_bytes[0],provenance.stack_bytes[1],provenance.stack_bytes[2],provenance.stack_bytes[3],provenance.stack_bytes[4],provenance.stack_bytes[5],provenance.stack_bytes[6],provenance.stack_bytes[7],provenance.stack_bytes[8],provenance.stack_bytes[9]);
  fprintf(stderr,"t198-s23 first-canonical observed=%u selector=%02x service=%02x host-int10-read=%u bytes=%02x%02x%02x%02x\n",t198_s23_native_ntio_boundary_observed_first_canonical_bop(),t198_s23_native_ntio_boundary_observed_first_canonical_selector(),t198_s23_native_ntio_boundary_observed_first_canonical_service(),t198_s23_native_ntio_boundary_observed_first_canonical_host_int10_read(),t198_s23_native_ntio_boundary_observed_first_canonical_host_int10(0),t198_s23_native_ntio_boundary_observed_first_canonical_host_int10(1),t198_s23_native_ntio_boundary_observed_first_canonical_host_int10(2),t198_s23_native_ntio_boundary_observed_first_canonical_host_int10(3));
  fprintf(stderr,"t198-s23 bios15 observed=%u state=%04x:%04x eax=%08x next=%02x composition=%u outcome=%u\n",t198_s23_native_ntio_boundary_observed_bios15(),t198_s23_native_ntio_boundary_observed_bios15_cs(),t198_s23_native_ntio_boundary_observed_bios15_eip(),t198_s23_native_ntio_boundary_observed_bios15_eax(),t198_s23_native_ntio_boundary_observed_bios15_next_byte(),t198_s23_native_ntio_boundary_observed_bios15_composition_handled(),t198_s23_native_ntio_boundary_observed_bios15_outcome());
  fprintf(stderr,"t198-s23 ioctl observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x\n",t198_s23_native_ntio_boundary_observed_ioctl(),t198_s23_native_ntio_boundary_observed_ioctl_cs(),t198_s23_native_ntio_boundary_observed_ioctl_eip(),t198_s23_native_ntio_boundary_observed_ioctl_eax(),t198_s23_native_ntio_boundary_observed_ioctl_ebx(),t198_s23_native_ntio_boundary_observed_ioctl_ecx(),t198_s23_native_ntio_boundary_observed_ioctl_edx());
  fprintf(stderr,"t198-s23 fast-read observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ebp=%08x flags=%08x ds-si=%04x:%04x es-di=%04x:%04x\n",t198_s23_native_ntio_boundary_observed_fast_read(),t198_s23_native_ntio_boundary_observed_fast_read_cs(),t198_s23_native_ntio_boundary_observed_fast_read_eip(),t198_s23_native_ntio_boundary_observed_fast_read_eax(),t198_s23_native_ntio_boundary_observed_fast_read_ebx(),t198_s23_native_ntio_boundary_observed_fast_read_ecx(),t198_s23_native_ntio_boundary_observed_fast_read_edx(),t198_s23_native_ntio_boundary_observed_fast_read_ebp(),t198_s23_native_ntio_boundary_observed_fast_read_eflags(),t198_s23_native_ntio_boundary_observed_fast_read_ds(),t198_s23_native_ntio_boundary_observed_fast_read_esi(),t198_s23_native_ntio_boundary_observed_fast_read_es(),t198_s23_native_ntio_boundary_observed_fast_read_edi());
  fprintf(stderr,"t198-s23 fast-read-commit observed=%u resume=%llx gpr16-mask=%08x ax=%04x flags-mask=%08x flags-value=%08x\n",t198_s23_native_ntio_boundary_observed_fast_read_commit(),t198_s23_native_ntio_boundary_observed_fast_read_resume_rip(),t198_s23_native_ntio_boundary_observed_fast_read_gpr16_write_mask(),t198_s23_native_ntio_boundary_observed_fast_read_ax(),t198_s23_native_ntio_boundary_observed_fast_read_eflags_write_mask(),t198_s23_native_ntio_boundary_observed_fast_read_eflags_values());
  for(unsigned n=0;n<t198_s23_native_ntio_boundary_fast_read_attempt_count();++n){struct t198_s23_fastread_attempt_v1 attempt; if(!t198_s23_native_ntio_boundary_copy_fast_read_attempt(n,&attempt))return 1; fprintf(stderr,"t198-s23 fast-read-attempt=%u accepted=%u state=%04x:%04x eax=%08x ebp=%08x ecx=%08x edx=%08x flags=%08x disposition=%u resume=%llx gpr16-mask=%08x ax=%04x flags-mask=%08x flags-value=%08x\n",n,attempt.accepted,attempt.cs,attempt.eip,attempt.eax,attempt.ebp,attempt.ecx,attempt.edx,attempt.eflags,attempt.disposition,(unsigned long long)attempt.resume_rip,attempt.gpr16_write_mask,attempt.ax,attempt.eflags_write_mask,attempt.eflags_values);}
  fprintf(stderr,"t198-s23 spckbd observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ds-si=%04x:%04x es-di=%04x:%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_spckbd(),t198_s23_native_ntio_boundary_observed_spckbd_cs(),t198_s23_native_ntio_boundary_observed_spckbd_eip(),t198_s23_native_ntio_boundary_observed_spckbd_eax(),t198_s23_native_ntio_boundary_observed_spckbd_ebx(),t198_s23_native_ntio_boundary_observed_spckbd_ecx(),t198_s23_native_ntio_boundary_observed_spckbd_edx(),t198_s23_native_ntio_boundary_observed_spckbd_ds(),t198_s23_native_ntio_boundary_observed_spckbd_esi(),t198_s23_native_ntio_boundary_observed_spckbd_es(),t198_s23_native_ntio_boundary_observed_spckbd_edi(),t198_s23_native_ntio_boundary_observed_spckbd_eflags());
  fprintf(stderr,"t198-s23 emm-probe observed=%u state=%04x:%04x eax=%08x ebx=%08x edx=%08x ds=%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_emm_probe(),t198_s23_native_ntio_boundary_observed_emm_probe_cs(),t198_s23_native_ntio_boundary_observed_emm_probe_eip(),t198_s23_native_ntio_boundary_observed_emm_probe_eax(),t198_s23_native_ntio_boundary_observed_emm_probe_ebx(),t198_s23_native_ntio_boundary_observed_emm_probe_edx(),t198_s23_native_ntio_boundary_observed_emm_probe_ds(),t198_s23_native_ntio_boundary_observed_emm_probe_eflags());
  fprintf(stderr,"t198-s23 mouse-install1 observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ds=%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_mouse_install1(),t198_s23_native_ntio_boundary_observed_mouse_install1_cs(),t198_s23_native_ntio_boundary_observed_mouse_install1_eip(),t198_s23_native_ntio_boundary_observed_mouse_install1_eax(),t198_s23_native_ntio_boundary_observed_mouse_install1_ebx(),t198_s23_native_ntio_boundary_observed_mouse_install1_ecx(),t198_s23_native_ntio_boundary_observed_mouse_install1_edx(),t198_s23_native_ntio_boundary_observed_mouse_install1_ds(),t198_s23_native_ntio_boundary_observed_mouse_install1_eflags());
  fprintf(stderr,"t198-s23 printer observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ds=%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_printer(),t198_s23_native_ntio_boundary_observed_printer_cs(),t198_s23_native_ntio_boundary_observed_printer_eip(),t198_s23_native_ntio_boundary_observed_printer_eax(),t198_s23_native_ntio_boundary_observed_printer_ebx(),t198_s23_native_ntio_boundary_observed_printer_ecx(),t198_s23_native_ntio_boundary_observed_printer_edx(),t198_s23_native_ntio_boundary_observed_printer_ds(),t198_s23_native_ntio_boundary_observed_printer_eflags());
  fprintf(stderr,"t198-s23 config-done observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ds=%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_config_done(),t198_s23_native_ntio_boundary_observed_config_done_cs(),t198_s23_native_ntio_boundary_observed_config_done_eip(),t198_s23_native_ntio_boundary_observed_config_done_eax(),t198_s23_native_ntio_boundary_observed_config_done_ebx(),t198_s23_native_ntio_boundary_observed_config_done_ecx(),t198_s23_native_ntio_boundary_observed_config_done_edx(),t198_s23_native_ntio_boundary_observed_config_done_ds(),t198_s23_native_ntio_boundary_observed_config_done_eflags());
  fprintf(stderr,"t198-s23 entry-dos-app observed=%u state=%04x:%04x eax=%08x ebx=%08x ecx=%08x edx=%08x ds-si=%04x:%04x di=%04x flags=%08x\n",t198_s23_native_ntio_boundary_observed_entry_dos_app(),t198_s23_native_ntio_boundary_observed_entry_dos_app_cs(),t198_s23_native_ntio_boundary_observed_entry_dos_app_eip(),t198_s23_native_ntio_boundary_observed_entry_dos_app_eax(),t198_s23_native_ntio_boundary_observed_entry_dos_app_ebx(),t198_s23_native_ntio_boundary_observed_entry_dos_app_ecx(),t198_s23_native_ntio_boundary_observed_entry_dos_app_edx(),t198_s23_native_ntio_boundary_observed_entry_dos_app_ds(),t198_s23_native_ntio_boundary_observed_entry_dos_app_esi(),t198_s23_native_ntio_boundary_observed_entry_dos_app_edi(),t198_s23_native_ntio_boundary_observed_entry_dos_app_eflags());
  return t198_s23_native_ntio_boundary_observed_first_generic_ud() && generic_valid &&
    generic.magic==BX_NTVDM_GENERIC_UD_EVENT_V1_MAGIC && generic.abi_version==BX_NTVDM_GENERIC_UD_EVENT_V1_VERSION && generic.struct_bytes==sizeof(generic) && generic.cs==0u && generic.eip==0x0047u && generic.execution_mode==1u && generic.vector==6u && generic.window_bytes>=4u && generic.window[0]==0xf0u && generic.window[1]==0x41u && generic.window[2]==0xf8u && generic.window[3]==0x00u &&
    t198_s23_native_ntio_boundary_observed_stop() && terminal_valid &&
    terminal.captured_physical_address==0x0deau && terminal.captured_byte_count==4u && history_count==8u && transition_valid && provenance_valid && provenance.predecessor_valid && provenance.stack_valid &&
    transition.previous.version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && transition.current.version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && transition.previous.sequence<transition.current.sequence && transition.previous.cs!=transition.current.cs &&
    history[7].version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && history[7].cs==0u && history[7].rip==0x0047u &&
    history[6].version==BX_NTVDM_INSTRUCTION_HISTORY_V1_VERSION && history[6].sequence+1u==history[7].sequence?0:2;
}

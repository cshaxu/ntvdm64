# M0 T270 S4 P6 — Opennt-bop complete source disposition ledger

## Scope and method

This ledger enumerates every production source, header and include input below
`src/opennt-bop` at the S4 review point.  It separates files with a selected
OpenNT comparison baseline from the BOP-owner composition sources which have no
upstream translation-unit counterpart.  The latter retain their BOP owner:
moving selector/service interpretation into selector-blind `adapter-bop`
would violate the architecture, while moving it to a private overlay would
create an impermissible public overlay consumer.

For an original-backed body, the component README is the local divergence
register.  The selected C-body comparison set has 34 translation units: one is
exact and the remaining 33 are below the 50-percent limit.  The selected
header/include set has 19 inputs: 17 exact and two below the threshold. The six
over-threshold bodies are private overlay implementations. For
BOP-owned composition, the source-policy/README divergence records and this
ledger are the provenance/disposition record; it is not claimed to be an
OpenNT mirror.

## Per-file disposition

### selected original / registered low-delta mirror (44)

- `command/cmd.h`
- `command/cmdconf.c`
- `command/cmddisp.c`
- `command/cmdenv.c`
- `command/cmdexec.c`
- `command/cmdexit.c`
- `command/cmdkeyb.c`
- `command/cmdkeyb.h`
- `command/cmdmisc.c`
- `command/cmdpif.c`
- `command/cmdpif.h`
- `command/cmdredir.c`
- `command/cmdsvc.h`
- `dem/dem.c`
- `dem/demdasd.c`
- `dem/demdasd.h`
- `dem/demdata.c`
- `dem/demdir.c`
- `dem/demerror.c`
- `dem/demfcb.c`
- `dem/demfile.c`
- `dem/demgset.c`
- `dem/demhndl.c`
- `dem/demioctl.c`
- `dem/demlabel.c`
- `dem/demlock.c`
- `dem/demmisc.c`
- `dem/demmsg.c`
- `dem/demmsg.h`
- `dem/demsrch.c`
- `dpmi/dpmi32.c`
- `dpmi/dpmi32p.h`
- `dpmi/dpmidata.h`
- `dpmi/dpmiselr.c`
- `dpmi/precomp.h`
- `dpmi/xlathlp.h`
- `xms/suballcp.h`
- `xms/suballoc.c`
- `xms/xms.c`
- `xms/xmsa20.c`
- `xms/xmsblock.c`
- `xms/xmsdisp.c`
- `xms/xmsmisc.c`
- `xms/xmsumb.c`

### BOP-owned source-derived seam (50)

- `command/opennt_command_binary_composition.c`
- `command/opennt_command_composition.c`
- `command/opennt_command_composition.h`
- `command/opennt_command_config_composition.c`
- `command/opennt_command_environment_composition.c`
- `command/opennt_pif_composition.c`
- `command/opennt_pif_composition.h`
- `dem/dem_direct_context.c`
- `dem/dem_direct_context.h`
- `dem/opennt_dem_ccpu_sas_facade.c`
- `dem/opennt_dem_ccpu_sas_facade.h`
- `dem/opennt_dem_common_compat.h`
- `dem/opennt_dem_dispatch_composition.c`
- `dem/opennt_dem_dispatch_composition.h`
- `dem/opennt_demdasd_ioctl_compat.h`
- `dem/opennt_demdasd_ioctl_composition.c`
- `dem/opennt_demdir_composition.c`
- `dem/opennt_demdir_composition.h`
- `dem/opennt_demerror_lock_composition.c`
- `dem/opennt_demerror_lock_composition.h`
- `dem/opennt_demfile_compat.h`
- `dem/opennt_demfile_composition.c`
- `dem/opennt_demfile_composition.h`
- `dem/opennt_demgset_compat.h`
- `dem/opennt_demgset_composition.c`
- `dem/opennt_demgset_composition.h`
- `dem/opennt_demmisc_compat.h`
- `dem/opennt_demmisc_composition.c`
- `dem/opennt_demsrch_fcb_compat.h`
- `dem/opennt_demsrch_fcb_composition.c`
- `dem/opennt_demsrch_fcb_composition.h`
- `dpmi/dpmi_descriptor_source_shim.c`
- `dpmi/dpmi_descriptor_source_shim.h`
- `dpmi/dpmi_startup_session_shim.c`
- `dpmi/dpmi_startup_session_shim.h`
- `dpmi/dpmi_startup_source_shim.c`
- `dpmi/dpmi_startup_source_shim.h`
- `opennt_ccpu_sas_facade.h`
- `softpc/mouse_install1_headless.c`
- `softpc/nt_bop_config_done.c`
- `softpc/opennt_config_complete_composition.c`
- `softpc/opennt_int06_provider.c`
- `softpc/opennt_int06_provider.h`
- `softpc/printer_openclose.c`
- `softpc/spckbd_handoff.c`
- `softpc/update_kbd_int15.c`
- `vdmredir/vrdisp_compat.h`
- `vdmredir/vrmslot_compat.h`
- `xms/opennt_xms_compat.h`
- `xms/opennt_xms_composition.c`

### private-overlay boundary (6)

- `dem/demdisp.c`
- `softpc/emm_fncs.c`
- `softpc/mem_size.c`
- `softpc/tape_io.c`
- `vdmredir/vrdisp.c`
- `vdmredir/vrmslot.c`

### exact selected original (9)

- `inc/bop.h`
- `inc/BOP.INC`
- `inc/cmdsvc.h`
- `inc/dbgsvc.h`
- `inc/dossvc.h`
- `inc/dpmi.h`
- `inc/rdrsvc.h`
- `inc/xmssvc.h`
- `softpc.new/host/src/nt_pif.c`

### BOP-owned composition route (52)

- `ingress/bios_selector_map.c`
- `ingress/bios_selector_map.h`
- `ingress/command_generic_ud_bridge.c`
- `ingress/command_generic_ud_bridge.h`
- `ingress/command_native_session.c`
- `ingress/command_native_session.h`
- `ingress/command_runtime_session.c`
- `ingress/command_runtime_session.h`
- `ingress/config_complete_generic_ud_bridge.c`
- `ingress/config_complete_generic_ud_bridge.h`
- `ingress/dem_direct_session.c`
- `ingress/dem_direct_session.h`
- `ingress/dem_generic_ud_bridge.c`
- `ingress/dem_generic_ud_bridge.h`
- `ingress/dem_ingress_shim.c`
- `ingress/dem_ingress_shim.h`
- `ingress/dem_native_session.c`
- `ingress/dem_native_session.h`
- `ingress/dem_runtime_session.c`
- `ingress/dem_runtime_session.h`
- `ingress/dem_startup_composition.c`
- `ingress/dem_startup_composition.h`
- `ingress/opennt_bop_route.c`
- `ingress/opennt_bop_route.h`
- `ingress/redir_generic_ud_bridge.c`
- `ingress/redir_generic_ud_bridge.h`
- `ingress/redir_native_session.c`
- `ingress/redir_native_session.h`
- `ingress/softpc_emm_unavailable_generic_ud_bridge.c`
- `ingress/softpc_emm_unavailable_generic_ud_bridge.h`
- `ingress/softpc_machine_interrupt_generic_ud_bridge.c`
- `ingress/softpc_machine_interrupt_generic_ud_bridge.h`
- `ingress/softpc_memory_size_generic_ud_bridge.c`
- `ingress/softpc_memory_size_generic_ud_bridge.h`
- `ingress/softpc_mouse_vector_generic_ud_bridge.c`
- `ingress/softpc_mouse_vector_generic_ud_bridge.h`
- `ingress/softpc_printer_openclose_generic_ud_bridge.c`
- `ingress/softpc_printer_openclose_generic_ud_bridge.h`
- `ingress/softpc_tape_io_generic_ud_bridge.c`
- `ingress/softpc_tape_io_generic_ud_bridge.h`
- `ingress/spckbd_handoff_generic_ud_bridge.c`
- `ingress/spckbd_handoff_generic_ud_bridge.h`
- `ingress/startup_machine_interrupt.c`
- `ingress/startup_machine_interrupt.h`
- `ingress/top_level_nosupport_generic_ud_bridge.c`
- `ingress/top_level_nosupport_generic_ud_bridge.h`
- `ingress/xms_generic_ud_bridge.c`
- `ingress/xms_generic_ud_bridge.h`
- `ingress/xms_native_session.c`
- `ingress/xms_native_session.h`
- `ingress/xms_runtime_session.c`
- `ingress/xms_runtime_session.h`

### BOP-owned observation composition (16)

- `observation/bop_sequence_observation.c`
- `observation/bop_sequence_observation.h`
- `observation/command_bootstrap_observation.c`
- `observation/command_bootstrap_observation.h`
- `observation/command_current_dir_observation.c`
- `observation/command_current_dir_observation.h`
- `observation/dem_open_observation.c`
- `observation/dem_open_observation.h`
- `observation/demfile_create_observation.c`
- `observation/demfile_create_observation.h`
- `observation/generic_ud_sequence_observation.c`
- `observation/generic_ud_sequence_observation.h`
- `observation/ntdos_exec_entry_observation.c`
- `observation/ntdos_exec_entry_observation.h`
- `observation/ntdos_exec_observation.c`
- `observation/ntdos_exec_observation.h`

## Audit result

- Production inputs enumerated: 177.
- Selected-original comparison set: 34 C bodies (one exact, 33 below the
  threshold) plus 19 headers/includes (17 exact, two below the threshold).
- Private overlay boundaries: six: DEM dispatcher; VDMREDIR dispatcher and
  mailslot body; and three reached SoftPC BIOS fragments.
- BOP-owned composition sources remain public component implementation by
  necessity.  They are not an extension point: no other component includes
  them, and they may not replace an imported provider without a separately
  registered source-derived exception.
- The external overlay-consumer scan must find only same-component mirror
  translation-unit includes.

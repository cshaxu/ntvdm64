# M0 T259 S1 — Current Facade Reconciliation

Status: S1 closure evidence  
Companion: [host/machine interface catalog](m0-t259-s1-host-machine-interface-catalog-001.md)

## Method

This review maps the active `src/bx-vdm` source-shaped facades to the catalog
without treating file count as a second API inventory.  A named shim remains
allowed only when it is the composition boundary of an imported OpenNT
translation unit.  Existing differences are retained only where they have an
explicit catalog disposition; similar names do not authorize a second generic
facility.

## Active facade groups

| Active files / group | Imported source owner | Catalog ID(s) | Current treatment | Reconciliation result |
| --- | --- | --- | --- | --- |
| `dem_direct_host_session`, `dem_direct_context`, `demdir`, `demfile`, `demgset`, `demdasd_ioctl`, `demerror_lock`, `demdisp`, `dem_ingress`, `dem_native_session`, `demmisc`, `demsrch_fcb`, `demhndl` | DEM | HOST-01, HOST-02, HOST-07, HOST-10, HOST-11, MACH-01/02/04 | Public Win32 calls plus bounded guest-memory/handle seams | Retain per imported owner; raw-media and `host_simulate` branches remain source failure/deferred or typed mechanics, not new host APIs. |
| `command_misc`, `command_config`, `command_environment`, `command_binary`, `command_native_session`, `command_pif`, `command_opennt_pif_parser` | COMMAND | HOST-03, HOST-04, HOST-05, HOST-06, HOST-07, HOST-10, HOST-11/12 | Source-shaped local process/session facade | Retain.  RTL helper spellings are presently distributed across these files; S2 must establish their one canonical declaration/ownership surface before moving bodies. |
| `redir_session` and imported `cmdredir.c` | COMMAND/Redirector boundary | HOST-03, HOST-08 | Local completion state only | Retain boundary, but Redirector protocol/provider remains deferred; it cannot imply `VDMREDIR` availability. |
| `xms_shim`, `xms_native_session` | XMS | HOST-07, HOST-11, MACH-02 | Checked real-mode copies and A20 mechanics | Retain.  UMB is a future opaque physical-span request; no UMB/DOS policy may move to mantle. |
| `dpmi_xmem_record_adapter`, `dpmi_startup_source`, `dpmi_startup_session`, `dpmi_descriptor_source` | DPMI/DOSX | HOST-11/12, MACH-01/02/04 | Bounded source imports with deferred protected lifecycle | Retain as explicitly incomplete source seams; no new frame ABI or `53:xx` enablement follows. |
| `softpc_memory_size`, `softpc_tape_io`, `softpc_printer_openclose`, `softpc_mouse_vector`, `softpc_emm_unavailable`, `softpc_int15_watch`, `spckbd_handoff` | SoftPC mirror | MACH-02/03/04/05/08/09 | Typed/checked machine facts behind `bx-vdm` | Retain.  Their implementation must never make `bx-core`/`bx-mantle` recognize SoftPC or BOP selectors. |
| `top_level_nosupport` | top-level BOP policy | HOST-05, MACH-07 | Session-level controlled stop/continue | Retain and keep BOP 59 source behavior separate from generic CLI ignore. |
| `bx_ntvdm_guest_pointer_manager`, `bx_ntvdm_host_handle_manager` | cross-family T258 seam | HOST-10, HOST-11 | Three fixed session mapping-manager instances | Retain as the sole admitted manager implementation.  No facade may allocate a private manager or expose host pointer bits. |
| `bx_ntvdm_machine_composition_v2`, `bx_ntvdm_machine_profile_abi`, startup machine interrupt and typed CPU-result/transaction ABI | composition, not an imported provider | MACH-01/03/04/08 | selector-blind typed transport | Retain pending S3 contract reconciliation; no additional CPU-frame ABI is admitted. |

## Duplicate and obsolete-spelling decision

1. The individual `Rtl*` implementations in the COMMAND shims are not an
   approved family of independent runtime facades.  They are one historical
   NTDLL/RTL surface (`HOST-07`) whose declarations and allocation/error
   contracts must be normalized in T259 S2.  Until then, S2 must not create a
   parallel generic utility library or alter source call order.
2. `GetVDMAddr` and `Sim32*` macro spellings may remain owner-local adapters
   because their source signatures differ; all are required to use the one
   T258 guest-memory mapping-manager instance.  Their persistence/protected
   lifetime limitations are not evidence for a new pointer manager.
3. `host_simulate`, `sas_*`, CCPU macros and `NtVdmControl` spellings are
   historical source names, not candidate global APIs.  Each maps only to the
   specific `MACH-*` route or deferred failure stated in the catalog.
4. No current active facade is deleted by this documentation-only closure.
   Deletion requires the importing translation unit to have a tested
   source-shaped replacement and all route/build references removed together.

## S1 result

All active facade groups now have a canonical interface-family disposition.
The next admissible action is S2: consolidate the declared OpenNT RTL/NTDLL
ABI contract and x86/x64 rules before changing any provider body.  It is not
admissible to create an independent CCPU, SAS, CSR/BaseSrv, VDD or generic
CPU-frame substitute.

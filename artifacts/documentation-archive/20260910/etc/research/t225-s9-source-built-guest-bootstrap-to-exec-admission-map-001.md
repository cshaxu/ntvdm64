# T225 S9 Source-Built Guest Bootstrap-to-EXEC Admission Map 001

## Question

Does the current source-built composition establish a guest-owned, continuous
path from the staged NTIO image through NTDOS and COMMAND to the original
`$Exec` eligibility point, and what is the smallest next owner package if it
does not?

## Inputs

- S8's four primary original-toolchain stages and the separate same-hash reference witness.
- Original OpenNT sources: `base/mvdm/dos/v86/doskrnl/bios/msinit.asm`, `sysinit1.asm`, `sysinit2.asm`, `base/mvdm/dos/v86/inc/mshead.asm`, `base/mvdm/dos/v86/doskrnl/dos/msproc.asm`, and `base/mvdm/dos/dem/demmisc.c`.
- Current composition: `bx_ntvdm_composition_runtime_v1.c`, `bx_ntvdm_boot_namespace_plane_v1.c`, `bx_ntvdm_dem_load_dos_service.c`, `bx_ntvdm_startup_session.c`, and `bx_ntvdm_startup_plan_abi.c`.
- T225 S1--S5 source maps and S6--S8 installation/build evidence.

## Original control and ownership path

| Transition | Original owner and contract | Host-service meaning |
| --- | --- | --- |
| Reset/startup to NTIO | NTIO startup is guest machine code; it reaches `msinit.asm:init`. | No DEM/COMMAND meaning belongs in Bochs or the adapter. |
| NTIO to NTDOS | `msinit.asm` derives a DOS load segment, places it in `DI`, then executes `SVC SVC_DEMLOADDOS`. | `demmisc.c:demLoadDos` obtains `GetVDMAddr(DI,0)`, opens `ntdos.sys`, repeatedly reads up to 16 KiB into that address, and terminates the VDM on open/read failure. |
| Post-load continuation | NTIO returns from the SVC and jumps to original `sysinit`/`goinit`. | Host only completes the write and resumes the original stream; it does not select a DOS entry or alter CPU state. |
| NTDOS initialization | `mshead.asm:$STARTCODE` starts with the original near jump to `DOSINIT`; `BioDataSeg` is `70h`. | NTDOS owns DOS data, MCB, PSP, INT 21h and process setup. |
| COMMAND bootstrap | `sysinit2.asm` retains guest `\\COMMAND.COM`; guest DOS chooses/opens it. | COMMAND services are source-mapped host requests; neither adapter nor mantle supplies an invented COMMAND entry. |
| Child entry eligibility | `msproc.asm:$Exec` owns validation, file reads, MCB/PSP creation and child transfer; immediately before guest transfer it invokes `SVC_DEMENTRYDOSAPP`. | S2's `50:36` is a void lifecycle notification, not a child launcher. |
| Child return | `msctrlc.asm:reset_environment` invokes `SVC_PDBTERMINATE` before guest cleanup and parent restoration. | S2's `50:3C` only releases provider-private host state, not PSP/MCB/JFN teardown or COMMAND return. |

## Current composition map

| Transition | Current bound route | Direct / Readonly disposition | Evidence status |
| --- | --- | --- | --- |
| Exact images admitted | `bx_ntvdm_composition_runtime_v1` exact-loads NTIO, NTDOS, COMMAND and declared target from one validated profile. | Both modes use locked image identity; unavailable modes reject before binding. | Installation only, proven by S6/S8. |
| NTIO initial placement | `prepare_startup_plan` writes NTIO at physical `0x700` and enters real-mode `CS:IP=0070:0000`; preserved state is `[0x714,4]`. | No host filesystem operation after exact image admission; identical in both modes. | ABI/install proven; execution unproven. |
| `50:11 demLoadDos` | Common ingress → DEM plane → boot namespace plane → `bx_ntvdm_dem_load_dos_service_v1_prepare`. It accepts only real-mode `C4 C4 50 11`, copies NTDOS to `(uint16_t)EDI << 4`, then resumes `RIP+4`. | Same preloaded image source. Malformed state/unavailable action controlled-stops rather than falling back to Direct. | Focused source/ABI route exists; post-resume continuity unproven. |
| NTDOS entry | No composition routine writes a NTDOS entry CS:IP; original NTIO continuation must reach it. | No mode distinction is valid here. | **Unproven.** |
| COMMAND bootstrap / `$Exec` | Current composition binds COMMAND and DEM/COMMAND packages, but S8 validates installation only. | Direct/Readonly selections install; no host child launch is selected. | **Unproven guest reach.** |

## Recovery ledger for `50:11`

| Recovery rung | Disposition |
| --- | --- |
| Original source reuse | `demmisc.c:demLoadDos` is normative for DI:0, sequential read and fatal failure, but needs historical CCPU/SAS register/memory access, VDD/debug state and product composition. It is not independently composable in MSVC x64. |
| Smallest adapter/shim | `bx_ntvdm_dem_load_dos_service_v1_prepare` retains copied NTDOS identity, DI-derived destination, one checked mechanical write and `RIP+4` resume. It has no filesystem lookup or DOS loader. |
| External-code intrusion | Rejected; no bx-core/bx-mantle change is needed. |
| New behavior | Rejected; no DOS entry, PSP, MCB, COMMAND loader or host process is created. |

## Missing proof and fixture gate

The remaining gap is not a missing image, selector, provider leaf or Bochs
device. It is integration evidence that native reset/entry, the `50:11`
write/resume and unchanged NTIO continuation form one guest execution sequence
that reaches the NTDOS header and then original initialization. Existing
fixtures prove installation and isolated provider ABI only.

The next package may build one exact-image **bootstrap-continuity fixture**:

1. stage only S8 primary manifest-bearing NTIO/NTDOS/COMMAND/SHARE inputs;
2. execute the existing MSVC x64 `/MT` bx-core/bx-mantle/bx-vdm graph without a new Bochs capability or BOP provider;
3. record fixed-width selector-blind machine/CPU facts around initial NTIO entry, `50:11` commit/resume and first NTDOS-owned entry;
4. treat the first earlier failure/controlled stop as owner-scoped evidence;
5. run Direct and Readonly separately, reject Overlay and never select Virtual;
6. stop before COMMAND or `$Exec` semantic repair. A reached command BOP is evidence for its existing package, not leaf-patch authority.

This fixture is not implemented or run by S9; its runtime scope requires its
own bounded package.

## Conclusion

S9 admits a bootstrap-continuity fixture as T225's next smallest package. The
ownership chain remains original: NTIO loads, NTDOS initializes, guest DOS
loads COMMAND and owns `$Exec`. Primary image availability and Direct/Readonly
selection are sufficient inputs; continuous execution is the missing fact.
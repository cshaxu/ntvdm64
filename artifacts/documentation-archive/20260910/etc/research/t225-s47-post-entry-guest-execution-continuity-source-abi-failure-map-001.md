# M0 T225 S47 — Post-Entry Guest Execution Continuity Source/ABI/Failure Map 001

## Question

S46 proves one accepted `50:36` no-VDD resume in an equal bounded
Direct/Readonly source-built run, but its COM-style `DS:SI=0c41:0100` does
not identify the selected MZ SHARE target; it observes neither `50:3C` nor
`54:0B`. Which components own the unproven path after that resume, and does
the current fact identify any BOP, provider, or machine repair?

## Original source control flow

| Phase | Original owner and source | Required original state/ordering | Current source-derived disposition |
| --- | --- | --- | --- |
| Pre-child notification | NTDOS `$Exec`, `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:1332-1371` | Loads `DS:SI` from `Exec_Init_IP`, `ES:DI` from `Exec_Init_SP`, copies `AX=ES`; `DX` is PDB and `BX` initial AX.  It emits `SVC_DEMENTRYDOSAPP` before changing user stack/registers. | One notification reached in both modes. Its COM-style entry geometry cannot assign it to selected MZ SHARE. |
| Host notification | Original `demEntryDosApp`, `src/opennt/base/mvdm/dos/dem/demmisc.c:286-318`; dispatcher `demdisp.c:153` | Reads PDB from DX; in the historic product only calls `VDDCreateUserHook(PDB)` when not first call, then returns void. | Current CLI deliberately has no VDD. `bx_ntvdm_dem_session_lifecycle_provider_v1.c:41-44` preserves the void result as checked `fault_rip + 4` resume. |
| Child entry transfer | NTDOS `$Exec`, `msproc.asm:1374-1389` | `DOCLI`; clear `InDos`; `mov SS,AX`; `mov SP,DI`; `STI`; push `DS:SI` as a far-return target; set `ES=DS=DX`; set `AX=BX`; `retf`. | Unproven. It is guest-owned code and must not be reconstructed in bx-vdm. |
| Child body | Original `share.asm` source-built as `TARGET.EXE` | `mov ah,4ch`; `xor al,al`; `int 21h`; `ret`. | Unproven. S46 deliberately does not substitute a host child or synthetic COM/EXE. |
| Guest teardown | NTDOS `$Abort` / `reset_environment`, `msproc.asm:1784` and `msctrlc.asm:918-1054` | PDB termination notification before arena release and `DOS_ABORT`; restore parent PDB, stack and terminate return vector; `DOIRET` returns to parent user terminate address. | Unproven. `50:3C` current provider only owns per-PDB search lifecycle once it is reached; it cannot replace guest teardown. |
| COMMAND return | Original `cmdReturnExitCode`, `src/opennt/base/mvdm/dos/command/cmdexec.c:590-649`; dispatcher `cmddisp.c:27` | Reads DX exit code, AL current drive and BX:CX redirector info; updates directories; asks `GetNextVDMCommand`; CF set means re-entry, CF clear means shell-out completion. | Unproven. Current `bx_ntvdm_command_lifecycle_provider_v1.c` retains the no-reentry / opaque-completion boundary but cannot become evidence before original parent restoration reaches `54:0B`. |

## Typed machine and adapter boundary

The accepted `50:36` route is mechanically narrow:

1. `bx-core/cpu/exception.cc:885-938` copies the pre-fault CPU state and a
   bounded instruction window into a selector-blind #UD event.
2. `bx-vdm/bx_ntvdm_vdm_generic_ud_bridge_v1.c:60-83` routes only after the
   current composition binds; S46's observer is after acceptance and is
   default-off.
3. `bx_ntvdm_dem_session_lifecycle_provider_v1.c:41-44` returns only the
   typed resume result at `fault_rip + 4` for service 36.  It does not write a
   general register, flags, segment, guest byte, service payload, or VDD
   state.
4. `exception.cc:931-938` applies only fields explicitly present in the typed
   outcome and resumes RIP.  For 50:36 there are no register/flag writes.
5. The original guest instructions listed above therefore remain the sole
   owner of SS:SP, DS/ES, AX and the far transfer to SHARE.

The finite mantle in `bx-mantle/bx_ntvdm_machine_stage_v1.cc:195-239` owns
only the minimal machine lifecycle, tick watchdog and cancellation.  It does
not interpret BOP selectors or guest lifecycle names.

## Static failure classification

The S46 raw output includes a later `BOUND_GdMa` diagnostic and repeated
`read_virtual_checks(): read beyond limit` diagnostics before the existing
budget terminal.  It contains no accepted `50:3C`, `54:0B`, first-fault
record, post-entry copied CPU state, or child-code observation.  Therefore it
cannot distinguish among these whole-component candidates:

| Candidate owner | What must be established before any repair |
| --- | --- |
| Source-built guest image/layout | The post-50:36 instruction stream and `$Exec` stack/segment inputs correspond to the original source-built image layout. |
| bx-core real-mode mechanics | Real-mode stack/segment/far-return and exception delivery follow the generic CPU contract for the source-selected instruction stream. |
| bx-mantle machine lifecycle | The minimal RAM, interrupt/timer and execution lifecycle supplies every actually reached native prerequisite without inventing DOS semantics. |
| Guest NTDOS teardown/COMMAND return | These remain downstream and cannot be diagnosed until child entry is independently established. |

No current evidence identifies an adapter BOP service defect: `50:36` is
accepted with its source-defined void-return contract.  No source evidence
identifies a missing device, FPU feature, IRQ route, or Bochs intrusion.  No
trace-selected implementation is admitted.

## Source-recovery ladder

1. **Original source reuse:** NTDOS `$Exec`, `$Abort`, `reset_environment`,
   original DEM notification and COMMAND return sources are present.  Guest
   code remains guest code, not host-linkable provider code.
2. **Smallest adapter seam:** current typed void-resume is the smallest seam
   compatible with original `demEntryDosApp` when the user-selected CLI profile
   has no VDD.  It is already present and no wider shim is justified.
3. **External Bochs intrusion:** none is selected.  Existing registered
   selector-blind #UD mechanics are an input to this audit, not authority for
   a new intrusion.
4. **New behavior:** rejected.  A synthetic loader, child process, guest
   injection, fake `50:3C`/`54:0B`, or adapter-owned stack transfer would
   bypass original owners.

## Binary identity correction

The immutable source-built NTDOS artifact contains two `C4 C4 50 36` sites
(offsets `0x3820` and `0x67e8`), corresponding to the normal and low-memory
transfer paths. The S46 record's `DS:SI=0c41:0100` is exactly the COM entry
shape stated by `$Exec`; it is not sufficient to name which guest executable
caused that invocation. The profile's selected `TARGET.EXE` is source-built
SHARE, an MZ image: its header has 32 header paragraphs, initial `CS=2`, and
initial `IP=16`; its executable payload begins at artifact offset `0x200`.
Consequently, a `DS:SI=PSP:0100` observation cannot be claimed as that EXE's
entry. Given the four-image profile, the record is compatible with initial
COMMAND.COM entry, but S47 retains this as a classification rather than a
proof of launch ordering.
## Image-layout and machine-stage boundary

The current composition does **not** host-load NTDOS, COMMAND or TARGET into a
fixed child address.  `bx_ntvdm_composition_runtime_v1.c:158-189` verifies and
retains all four manifest-bound images, but
`bx_ntvdm_composition_runtime_v1_prepare_startup_plan:347-366` writes only
NTIO to guest physical `0x700` and begins at `0070:0000`.  NTDOS, COMMAND and
the target are thereafter reached through the original guest-owned DOS
namespace/file/EXEC path.  That is consistent with the observed original
`$Exec` contract `DS:SI=0c41:0100`; it is not evidence for an adapter loader
or a missing host-side target placement action.

The machine-stage contract independently creates the minimal machine,
applies only the admitted initial-state and startup mechanical actions,
preserves the declared bytes, and arms the real-mode entry
(`bx_ntvdm_machine_stage_v1.cc:73-172`).  It has no authority to synthesize
the child stack, PSP, arena, COM load segment or far-return frame.  Any later
layout proof must therefore establish the guest-produced state against the
original NTDOS path, rather than adding a host write to make `0c41:0100` look
plausible.
## S47 disposition

S47 has established that the next necessary evidence, if any, must be a
whole post-entry mechanical-continuity proof tied to the original `$Exec`
transition.  It may not be an endpoint BOP implementation.  Before an
observer or repair is proposed, the remaining code/image layout and generic
real-mode mechanics must be reconciled as one owner package; otherwise the
correct disposition remains the explicit S46 bounded limitation.
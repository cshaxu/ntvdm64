# T225 S1 P1 Guest DOS Multi-Program Owner Boundary Map

## Question

Which original components own the first CLI profile's guest DOS parent/child
execution contract, and does any current adapter route permit treating a host
child process or an isolated BOP as its substitute?

## Inputs

- Queue candidate: Guest DOS multi-program execution closure.
- `docs/etc/research/proposal-cli-noninvasive-capability-roadmap-001.md`,
  "Guest DOS multi-program execution".
- OpenNT guest sources:
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm`;
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/misc.asm`;
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm`;
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/abort.asm`;
  - `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msdisp.asm`.
- Current `src/bx-vdm` DEM plane, package-session, process-owner and lifecycle
  sources.

## Procedure

1. Trace original `$Exec` from function validation through binary selection,
   environment, allocation, image load, PSP construction and transfer.
2. Trace the child termination path through `$Exit`/`$Abort` and
   `reset_environment` back to the parent PDB.
3. Identify every reached DEM/COMMAND notification and compare it with the
   current adapter's actual provider disposition.

## Observations

| Concern | Original owner and retained contract | Current route/disposition | Consequence |
| --- | --- | --- | --- |
| INT 21h `AH=4Bh` EXEC | `doskrnl/dos/msproc.asm:$Exec` | No admitted multi-program package route | Guest DOS owns function validation (`AL=0/1/3/5`), error returns, COM/EXE/overlay distinction and execution transfer. |
| Binary/file acquisition | `$Exec` calls `get_binary_type`, `$READ`, `$Lseek`, `$Close` and preserves DOS failures | Existing DEM file provider is a host capability for individual DOS file services, not an EXEC loader substitute | The first implementation must prove that the guest's existing file-handle/SFT path can load a selected guest child. |
| Environment | `$Exec` scans at most 32 KiB, allocates/copies the chosen or inherited environment, appends image name, and deallocates on failure | No adapter environment copy may replace this guest algorithm | The 4023-byte COMMAND bootstrap limit is unrelated; EXEC has its own original environment limit and failure flow. |
| MCB and image memory | `$Exec` uses `$Alloc`, `ChangeOwner`, `arena_free_process`, relocation and COM/EXE load rules | bx-core/mantle machine RAM remains mechanical only | No adapter or Bochs change is admitted; the guest DOS kernel must own MCB/PSP memory state. |
| PSP creation and inheritance | `misc.asm:$Dup_PDB` / `$CREATE_PROCESS_DATA_BLOCK` copy parent PSP and JFN state, respect no-inherit/network handles, set `PDB_Parent_PID`, then set `CurrentPDB` | Current adapter has only bounded PDB identity reads for selected DEM namespace ownership | A copied PDB segment is not a guest process implementation. The source-owned PSP/JFN path must run. |
| Child entry | `$Exec` writes `PDB_Exit`, `ADDR_INT_TERMINATE`, DMA and initial registers, calls `SVC_DEMENTRYDOSAPP`, then transfers in guest real mode | `SVC_DEMENTRYDOSAPP` is an old DEM host notification, not a host child launch authority | Entry observation must remain non-semantic unless separately admitted. |
| Child termination | `$Exit`/`$Abort` restore vectors then transfer to `msctrlc.asm:reset_environment` | No current route proves parent/child return | This is a required half of the package, not a follow-up after launch succeeds. |
| Cleanup and parent return | `reset_environment` checks `CurrentPDB`, sends `SVC_PDBTERMINATE`, calls `arena_free_process` and `DOS_ABORT`, restores parent `CurrentPDB`, parent stack and terminate address, then `DOIRET`s | Current `50:60` provider releases only bounded per-PDB search state | `50:60` is a necessary notification but cannot be treated as EXEC/return closure. |
| Host child BOPs | T224's `54:08/0A/0B` provider is a separate COMMAND host-child capability | Explicitly non-pipe, profile-gated, and not guest DOS EXEC | Nested host NTVDM/process launch is forbidden for T225. |

## Source-Recovery Disposition

| Rung | Result |
| --- | --- |
| Original source | Required guest owner: the source is present in the OpenNT DOS kernel and already defines the algorithms, layouts, ordering and failure behavior. |
| Smallest adapter/shim | Not yet selected. A later seam may only provide already-required DOS file/device host capability; it cannot implement EXEC, PSP, MCB, relocation or parent return. |
| External-code intrusion | Rejected at S1: no Bochs intrusion is evidenced. |
| New behavior | Rejected at S1: a new adapter EXEC/process model would violate the original-owner boundary. |

## Interpretation And Confidence

High confidence that T225 must be a guest-kernel execution package, not a
COMMAND BOP extension and not a `bx-vdm` process reimplementation. The current
`50:60` route is correctly narrow: it handles only `demTerminatePDB`'s
search-state notification after the original guest cleanup path reaches it.

The capability-roadmap common admission gate remains unproven: the first DOS
profile has not yet produced evidence of a declared DOS target reaching normal
guest termination through the full current BOP package set. Therefore S1 is
still an admission audit. No EXEC implementation, host nested launch, Bochs
change, or native trace is authorized by this record.

## Follow-Up

Map the concrete kernel build/artifact closure and each `$Exec` file-service
prerequisite against the Direct and Readonly provider dispositions; then map
normal termination's terminal result path. Only after those two maps can S1
state whether T225 may enter a source-built parent/child/return fixture phase.
## P2 EXEC File-Path And Terminal-Prerequisite Comparison

### Procedure

The original `$Exec:get_binary_type` and `ExecRead` calls were traced through
`$Open`, `$Lseek`, `$Read` and `$Close`, then compared with the current
`bx_ntvdm_dem_package_session_v1` dispatch ordering and its Direct/Readonly
provider partitions. Retained T203 normal-terminal evidence was reviewed
rather than inferred from the existence of a `54:0B` handler.

### EXEC File Contract

| Original guest operation | DEM BOP | Current bound route | T225 relevance |
| --- | --- | --- | --- |
| `$Open` from `get_binary_type` | `50:12` / `SVC_DEMOPEN` | Whole-provider namespace partition; Direct opens host-view objects, Readonly opens declared startup images | Required; it must create the source-shaped SFT/JFN handle state which `$Exec` later consumes. |
| `$Lseek` | `50:00` / `SVC_DEMCHGFILEPTR` | Whole-provider handle partition | Required for EXE relocation and COM/EXE load positioning. |
| `$Read` slow path | `50:16` / `SVC_DEMREAD` | Whole-provider handle partition; checked guest RAM transaction | Required fallback and image-byte transfer. |
| `$Close` | `50:02` / `SVC_DEMCLOSE` | Whole-provider handle partition | Required on all `$Exec` success/error paths. |
| `$Read` fast path | `50:42` / `SVC_DEMFASTREAD` | Boot namespace provider calls only its readonly startup-file service | **Not closed for Direct child images.** |

`handle.asm:FastOrSlow` selects the fast path for every non-pipe read unless
the MIPS bit is set. The x86 CLI profile therefore reaches `50:42` before
`50:16` for a normal child image. In the historical flow, a carry result from
FastRead deliberately falls through to `SVC_DEMREAD`. The current `50:42`
provider is source-derived for declared readonly startup images; it neither
owns an admitted Direct-token route nor records a source-shaped Direct
carry-set fallback. Consequently a Direct child EXEC cannot presently be
claimed to reach its existing `50:16` loader path.

The existing Direct `50:12` namespace path also requires its copied
`direct_namespace_owner`, which the session obtains from the registered DTA's
CurrentPDB read. This may be a valid bounded host-file ownership seam, but no
current evidence proves that the real `$Exec` sequence has established that
registration and source-shaped SFT/JFN state before it invokes `50:12`.

### Normal-Terminal Comparison

T203's final evidence is controlling: its source-built observer did **not**
see a valid `54:0B` normal return on the current finite fixture, while the
product engine instead reached `50:3D`/`demExitVDM` and a fatal `config.nt`
path. `ORDINARY_GUEST_COMPLETION` has no engine producer. Existing local
`54:0B` code and a diagnostic observer therefore do not satisfy the roadmap's
"real declared target through normal guest termination" gate.

### Disposition

S1 remains unclosed. The next package plan must first admit one complete
**guest EXEC file I/O compatibility subpackage**, rather than add a child
launch shortcut:

1. prove the original SFT/JFN token layout against `50:12/00/42/16/02`;
2. select exactly one Direct `50:42` disposition that preserves original
   carry-to-`50:16` fallback or source-equivalent Direct fast read;
3. establish the real CurrentPDB/DTA prerequisite with a guest-path fixture;
4. only then admit COM first, EXE relocation second, and parent return/terminal
   result last as one parent/child/return package.

No direct code change, Bochs change, host process, nested VDM, or trace-led
BOP patch is admitted by P2.
## P3 Token ABI And FastRead Source Boundary

### Token ABI Is Shared And Profile-Neutral

OpenNT `dem.h` defines `GETHANDLE(hi, lo)` as `GETULONG(hi, lo)`.  The DOS
kernel writes `AX:BP` to `sf_NTHandle+2:sf_NTHandle` after `SVC_DEMOPEN`, and
later reloads that same `AX:BP` pair for `SVC_DEMCHGFILEPTR`, `SVC_DEMREAD`,
`SVC_DEMFASTREAD`, and `SVC_DEMCLOSE`.  The current namespace partition
returns token high to AX and token low to BP for Direct, Readonly, Overlay,
and Virtual; its file-session mapping retains host HANDLEs privately.

The existing whole-provider session fixture proves a bounded DTA/PDB
registration, `50:12 -> 50:16 -> 50:02` chain for Direct and Readonly, and
uses the same opaque token representation.  Thus the shared token ABI is a
valid seam to retain for all four profiles; no profile may substitute a raw
host HANDLE or a different guest register layout.

### FastRead Is Not A Proven Original Direct Provider

`handle.asm` branches to `SVC_DEMREAD` if `SVC_DEMFASTREAD` returns CF set.
However the retained OpenNT `demdisp.c` maps `SVC_DEMFASTREAD` to
`demNotYetImplemented`, whose retained body clears CF.  This source tree
therefore proves the *control-flow fallback exists*, but does not prove that
the historical dispatcher selected that fallback for normal non-pipe reads.
It also cannot by itself prove a successful Direct fast-read byte-transfer
contract.

The current `50:42` source-derived provider is deliberately narrower: it
performs the copied fast-read contract only for the readonly declared startup
namespace.  It is not evidence for Direct.  The Direct package must therefore
choose and test one explicit result contract before any real EXEC admission:

1. a typed CF-set disposition that demonstrably reaches the original `50:16`
   path with the required preserved registers; or
2. a Direct fast-read provider that uses the existing private token and checked
   guest-RAM transaction ABI, with source-derived evidence for every result
   register and SFT-position effect.

Neither choice changes the shared profile ABI.  Overlay and Virtual remain
future providers behind that ABI and receive no implied implementation by this
record.

### Corrected Admission Result

P2's statement about the `50:16` fallback denotes the DOS kernel branch only,
not an established original-host FastRead failure disposition.  The core P2
conclusion remains: Direct EXEC is unadmitted until FastRead's Direct result
contract and the real CurrentPDB/DTA guest path are closed.  No implementation
or native trace is authorized by P3.
## P4 Reproducible Direct/Readonly Shared-Route Witness

The existing `dem-whole-provider-session` fixture was rebuilt in the fresh
`build/M0-T225-S1/001` disposable root with MSVC x64 `/MT`.  Its manifest
reported `compileExitCode=0`, `linkExitCode=0`, `runExitCode=0`, and
`passed=true`.  Its source closure explicitly excludes `bx-core`, `bx-mantle`,
Bochs binaries, an OpenNT runtime and a CLI engine.

The fixture drives real-mode `C4 C4 50 xx` through ingress, registry and DEM
package-session dispatch.  For both Direct and Readonly it proves the bounded
DTA/PDB transport, `50:12` open, `50:16` checked guest-RAM read and `50:02`
close using the same opaque `AX:BP` token.  It also proves released tokens
return the source-shaped invalid-handle result rather than falling through to
a profile default.  Readonly separately refuses its write/mutation surface by
capability.

This witness is deliberately limited: it does not execute guest `$Exec`, it
does not dispatch Direct `50:42`, and it does not prove terminal return.  It
therefore corroborates P3's shared-ABI conclusion but leaves the T225 package
gate unclosed.  The disposable build root may be removed after this evidence
record; no build product is an acceptance artifact.
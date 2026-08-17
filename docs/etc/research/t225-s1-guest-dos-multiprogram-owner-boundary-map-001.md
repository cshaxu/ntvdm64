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
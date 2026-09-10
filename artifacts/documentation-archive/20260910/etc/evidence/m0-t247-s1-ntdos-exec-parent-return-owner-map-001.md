# M0 T247 S1 — NTDOS guest EXEC and parent-return owner/ABI map

## Question

Which part of a source-built DOS `EXEC` → child → ordinary parent-return
profile is already original guest code, which existing BOP contracts it
consumes, and what is the smallest next composition step that can add evidence
without recreating DOS process handling in `bx-vdm`?

## Inputs

- Primary original-toolchain source-built inputs are fixed by
  `tools/build/Stage-T228S1GuestExecIntegrationInputs.ps1`: `NTIO.SYS`
  (33792 bytes, SHA-256 `cfc8be16576bb6acd16bb8fc9b2d9a080f544bbfdd2d2d2df07ed908b3ab4937`),
  `NTDOS.SYS` (27858 bytes, SHA-256
  `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84`),
  `COMMAND.COM` (50384 bytes, SHA-256
  `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`),
  and an unchanged source-built `SHARE.EXE` test target.
- Original guest source: `src/opennt/base/mvdm/dos/v86/doskrnl/dos/` and
  `src/opennt/base/mvdm/dos/v86/inc/`.
- Existing host endpoints: imported `demmisc.c` (`50:36`) and imported
  COMMAND `cmdexec.c` / `cmddisp.c` (`54:07/08/0A/0B`).

## Original lifecycle and ownership

| Transition | Original source and retained behavior | Current composition disposition |
| --- | --- | --- |
| INT 21h AH=4Bh dispatch / function validation | `msproc.asm:$Exec` accepts only AL `0`, `1`, `3`, `5`; AL `5` preserves the special `LeaveDos` A20 rule. | **Guest-owned and already staged.** No adapter implementation is allowed. |
| Binary classification | `msproc.asm:get_binary_type` performs `$OPEN/$IOCTL/$READ/$LSEEK` header classification. An unknown result alone invokes `SVC_CMDCHECKBINARY` (`54:07`). | The first local DOS-binary profile uses the source AL=0 path and does not require `54:07`; unknown/32-bit selection remains the existing COMMAND host contract. |
| Environment and executable allocation | `$Exec` uses the supplied environment or `CurrentPDB:PDB_environ`, then its original allocation/error branches. `alloc.asm` owns arena validity and `arena_free_process`; `arena.inc` fixes the `M/Z`, owner and paragraph layout. | **Guest-owned and already staged.** A host allocation, adapter arena object, or synthetic PSP would violate the source boundary. |
| Child PDB/PSP creation | `$Exec` calls `$Dup_PDB`; `misc.asm:Create_dup_jfn` copies inherited JFNs, sets `PDB_Parent_PID`, changes `CurrentPDB`, and preserves the fixed `pdb.inc:Process_data_block` layout. `$Exec` then writes `PDB_environ`, FCBs, command tail and `PDB_Exit`. | **Guest-owned and already staged.** The existing fixed 50-byte copied observer is read-only and covers only `PDB_Exit`, Ctrl-C/fatal vectors, parent PDB and environment; it is not a PDB implementation. |
| Child entry | `$Exec:Xfer_To_User` emits `SVC_DEMENTRYDOSAPP` (`50:36`) with entry/stack/PDB facts, then restores the child stack/registers and `retf`s into guest code. The HMA branch remains `DisA20_Xfer`/`EXECA20OFF` machine work. | The imported `demmisc.c:demEntryDosApp` is active through `demmisc_shim.c`; non-debug `VDDCreateUserHook` is a no-op shim, preserving return. The ordinary non-HMA first profile is composable. |
| Child termination and parent restoration | `$Exit/$Abort/Abort_Inner` retain exit type/code, restore saved vectors, then transfer to `reset_environment`; `abort.asm` closes JFNs and `alloc.asm` releases child arenas. The debug-only `50:35` notification is conditional source behavior. | **Guest-owned and already staged.** Debug symbol lifecycle transfers to Debugger/VDD; Redirector owns remote JFN semantics. |
| Host-child return | COMMAND `54:08/0A/0B` is the already closed single-session host-child worker contract. It may provide a result to the existing guest-facing branch, but it does not replace the guest DOS PDB/parent return machinery. | Existing original-source COMMAND provider; this S does not rewrite it. |

## ABI facts and boundary decision

`pdb.inc` fixes the first-profile PDB fields: exit vector at offset 10,
Ctrl-C at 14, fatal-error at 18, parent PDB at 22, environment at 44, plus
the original JFN count/pointer fields.  `bx_ntvdm_ntdos_exec_observation_v1`
already validates a 16-bit PDB segment, requests exactly 50 checked guest-RAM
bytes, and decodes those fields after the caller-owned copy.  It has no guest
pointer, allocation, write, DOS dispatch or host-handle operation.

The source-defined place to take that read-only snapshot is the successful
`50:36` handoff: `$Exec` has completed `$Dup_PDB` and saved child return
state, while the original guest has not yet changed to the child stack and
entry.  The next S may therefore attach an **opt-in, fixed-size observation**
to the already imported `demEntryDosApp` route, using only its copied `DX`
PDB segment and the existing checked-RAM action.  It must not observe an
arbitrary BOP, inspect unbounded guest memory, or alter any guest register.

## Owner transfers

- HMA/A20-off transfer (`DisA20_Xfer`, `EXECA20OFF`) remains a pure
  bx-core/bx-mantle machine prerequisite.
- Remote/inheritable redirector JFNs and named-pipe completion remain the
  Redirector owner package.
- Unknown/NE/PE/LE classification branches retain the existing COMMAND/WOW
  ownership; they are outside the local DOS executable profile.
- Debug-only `50:34/50:35` module notifications transfer to Debugger/VDD.
- Actual CPU execution through a child and back to `reset_environment` is S3
  work after the S2 handoff snapshot proves its exact pre-child state.

## Decision

S2 is bounded: connect the existing fixed PDB observation to imported
`50:36 demEntryDosApp`, add positive/negative local tests, and prove it does
not alter the source return/resume contract.  This is a composition and
evidence seam, not a new BOP provider or a DOS implementation.  It needs no
Bochs change and no new host capability.

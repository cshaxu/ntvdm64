# M0 T248 S1 — post-EXEC real-mode continuity owner map

## Question and bounded input

The T247 source-built native checkpoint proves the original NTDOS
`$Exec:Xfer_To_User` handoff through accepted `50:36`, and its read-only
observer proves the child PDB at `0BF2h`. The same bounded run subsequently
accepts imported `50:1E` (`SVC_DEMWRITE`) and ends as machine-stage terminal
`UNEXPECTED_LOOP_RETURN` (`detail=6`). This map assigns that terminal without
selecting a new BOP provider.

The pinned guest inputs and their identities are those in
`m0-t247-s3-source-built-exec-entry-checkpoint-001.md`; this record adds no
guest bytes, provider semantics, or machine change.

## Source and ABI chain

| Edge | Evidence | Owner and result |
| --- | --- | --- |
| Original child handoff | `src/opennt/base/mvdm/dos/v86/msdos/msproc.asm:$Exec` builds the child PDB then transfers through `$Exec:Xfer_To_User`; T247 observes its `50:36` host handoff. | NTDOS guest. bx-vdm may service the host call but may not construct a PDB, PSP, arena, or parent return. |
| Normal or abort return | The same original guest package owns `$Exit`, `$Abort`, environment restoration and parent-PDB restoration; source locations and field map are retained in T247 S1 evidence. | NTDOS guest. It is not a COMMAND/DEM worker return contract. |
| `50:1E` after entry | `src/opennt/base/mvdm/inc/dossvc.h` names `1Eh` `SVC_DEMWRITE`; the active imported `demhndl.c:demWrite` route accepts it and returns a typed `RIP+3` resume. | bx-vdm OpenNT DEM provider. It is already code-complete and is not implicated by an eventual CPU-loop return. |
| SoftPC selector `06` | `src/opennt/base/mvdm/softpc.new/base/bios/bios.c` maps BOP `06` to `illegal_op_int`; `base/system/illegalp.c` rewrites the saved real-mode return IP and calls `unexp_nt.c:unexpected_int`. The active typed provider performs only those copied stack/RAM/PIC/BDA operations. | Existing bx-vdm machine-composition ingress, using selector-blind mantle RAM/port actions. It is neither a DEM nor COMMAND leaf. |
| CPU loop exit | `src/bx-core/cpu/cpu.cc:BX_CPU_C::cpu_loop` returns only when asynchronous event processing asks it to return. `src/bx-core/cpu/event.cc:handleAsyncEvent` returns for the finite kill latch; `handleWaitForEvent` also returns if a halted CPU sees that latch. | Adopted bx-core mechanic. |
| finite-stage result | `src/bx-mantle/bx_ntvdm_machine_stage_v1.cc:bx_ntvdm_machine_stage_v1_execute` classifies first-fault, typed stop, pending, cancellation, watchdog budget; only the remaining `cpu_loop()` return is `EXECUTION_UNEXPECTED_LOOP_RETURN`. | bx-mantle lifecycle classification. `detail=6` does **not** encode a BOP result or DOS error. |

## Eliminated and retained hypotheses

* A typed `50:1E` resume does not set `kill_bochs_request`; the generic
  exception bridge only sets that latch for typed stop or pending. Therefore
  a later `detail=6` is not evidence that `demWrite` failed to resume.
* The finite watchdog and cancellation callbacks set both their own recorded
  state and the same kill latch. If either callback fires, stage classification
  is budget or cancellation rather than `detail=6`.
* The only other adopted source assignment to that latch is the normal Bochs
  benchmark timer. It is not armed by the finite stage. A direct test must
  therefore distinguish HLT/event-return from an unrecorded lifecycle stop
  rather than enabling devices or changing a BOP provider.

## S2 replay selection

The smallest already-adopted diagnostic surface is a fresh formal build with
the default-off `SoftwareInterruptDiagnostic` and `InterruptReturnDiagnostic`
options. It already records fixed-width, selector-blind CPU facts:

* `src/bx-core/cpu/soft_int.cc` records software INT vector, real-mode
  registers and position only when enabled;
* `src/bx-core/cpu/ctrl_xfer16.cc` and `ctrl_xfer32.cc` record IRET position
  and operand width only when enabled;
* `src/bx-mantle/bx_ntvdm_{software_interrupt,interrupt_return}_observation_v1.cc`
  retain bounded copied records; and
* `src/cli/ntdos64_native_cli.c` exposes opt-in reporting flags.

S2 will rebuild the formal graph with only those two diagnostics, run the
same T247 source-built bounded input once, and compare the captured INT/IRET
sequence with the `06` stack frame and final CPU position. The test will not
add a selector recognizer, dereference guest policy data, enable a new device,
or assign guest `$Exit/$Abort` behavior to bx-core or bx-mantle.

## Tracker disposition

`BOP-DEPENDENCY-115` remains **partial**: T247 proves entry to original guest
child execution; this S1 map proves that the remaining terminal is a
selector-blind continuity investigation. S2 is the next admissible action.

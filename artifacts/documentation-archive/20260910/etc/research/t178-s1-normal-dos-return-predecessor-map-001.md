# T178 S1 normal DOS-return predecessor map 001

## Question and method

This read-only map asks what must occur between the currently observed
`50:36` BOP and a normal COMMAND `54:11` return boundary. It reconciles
original NTDOS/DEM/COMMAND source with the retained T141, T142, T176 S20 and
T177 S4 observations. No target image, runtime, provider or transport is
changed.

## Source and trace map

| Order | Boundary | Owner | Evidence and disposition |
| --- | --- | --- | --- |
| 1 | `50:36` / `SVC_DEMENTRYDOSAPP` | Original DEM notification | `demdisp.c` maps it to `demEntryDosApp`; absent a VDD hook that function returns. T177 S4 observes its accepted resume. It is not a result or exit service. |
| 2 | `$Exec` application transfer | Original NTDOS guest | `msproc.asm` executes `DOCLI`, clears `InDos`, installs the user stack and PDB registers, then `retf`s to DS:SI. This is the observed target-entry predecessor. |
| 3 | Target normal termination | Original guest DOS | A normal fixture would issue INT 21h/AH=4Ch. The currently retained execution does not prove this event for its target. |
| 4 | PDB termination and teardown | Original NTDOS guest | `msctrlc.asm` emits `50:3C`, frees the process arena, restores parent state and returns to the parent context. T176 S20 observes that guest cleanup follows `50:3C`; `54:11` is absent. |
| 5 | COMMAND normal-return service | Original COMMAND host composition | `tcode.asm` passes the child status in DX and emits `C4 C4 54 11`; `cmdReturnExitCode` then applies historical event-thread, directory, redirection and next-command logic. This BOP remains unobserved. |
| 6 | CLI terminal result | Future engine/CLI boundary | No owner is admitted. Bochs exit code is a simulator process result, not the guest DX status or historical COMMAND AL state. |

## Key distinctions

`50:36` has a source-defined `VOID` notification contract and occurs before
the target begins; it cannot establish target completion. Conversely, `54:11`
is reached only after guest termination and original DOS teardown restore
COMMAND execution. The original host handler mixes the guest DX code with
invasive GetNextVDMCommand, redirection, directory and thread machinery. The
contained no-next branch recorded by T173/T174 is therefore not evidence of a
runtime-normal return, and it must not be promoted into CLI status transport.

## Result

The first unproven edge is **target normal termination through original DOS
teardown to COMMAND `54:11`**. T177 S4 only proves the pre-target handoff.
No BOP provider, Bochs mechanism, adapter result, device, or CLI process-exit
mapping is implicated by this gap. S2 may only reconcile this map with the
available bounded traces and classify the most specific missing predecessor.

## Confidence

High for the source ownership/order through `50:3C` and for the distinction
between guest status and process exit. Medium for any particular target's
future normal-return reachability because no retained current trace reaches
`54:11`.

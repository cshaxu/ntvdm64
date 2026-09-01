# Proposal: CPU40/NTDOS post-`54:05` startup continuity

## Question

What is the smallest complete original CCPU40/NTDOS owner cohort required for
the fixed normal runtime container to continue from original
`CmdDispatch/cmdSetInfo` into the NTDOS `msinit.asm` continuation?

## Evidence baseline

Existing DOS and Win16 media are already consumed through the original
activation path.  T351's fixed container observes original `50:11`, `50:3B`,
`50:0F`, `50:1B`, and `54:05`, then its immutable eight-second watchdog ends
the child with `0x53504354`.  This neither proves `cmdSetInfo` return nor
identifies a missing guest loader or failed BOP provider; see [T351 S4 fixed
observation](../evidence/m0-t351-s4-fixed-container-bootstrap-observation-001.md).

## Scope

- Freeze the same normal console-owning container and selected media.
- Walk source and guest continuation from `cmdSetInfo` into NTDOS `msinit.asm`,
  including CCPU40 frame/segment/stack state, SAS memory, BIOS exchange and
  any actually reached FDC/INT15/heartbeat/PIT/ICA predecessor.
- Recover only by original source, same-shaped binding or registered minimal
  overlay. Do not use the trace to choose a BOP service change.
- Preserve load-only guest media and original NTDOS/COMMAND ownership; use the
  existing session mapping manager for guest spans.
- Require the selected CPU40/x86 formal compile/link evidence.  x64 is not a
  current acceptance row; architecture-neutral mapping-manager rules remain
  mandatory wherever an identity crosses the MVDM boundary.

## Exclusions

No guest loader, synthetic BOP success, DOS provider rewrite, new machine,
alternate executor, Bochs route, kernel-VDM/CSRSS recreation, generic WOW
callback scheduler, `EXEC`/PSP/child return, GUI/WOWEXEC, VDD or cross-process
work.

## Completion record

An admitted packet identifies the first missing or wrong original owner edge
after `54:05`, records its complete reached sibling cohort, and proves the
unchanged container reaches the next original guest ingress or exact
source-owned terminal result. It explicitly distinguishes this startup result
from DOS program execution.

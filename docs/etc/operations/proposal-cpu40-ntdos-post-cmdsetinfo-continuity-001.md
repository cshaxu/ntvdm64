# Proposal: CPU40/NTDOS post-`54:05` startup continuity

## Question

What is the smallest complete original CCPU40/NTDOS owner cohort required for
the fixed normal runtime container to continue from original
`CmdDispatch/cmdSetInfo` into the NTDOS `msinit.asm` continuation?

## Evidence baseline

Existing DOS and Win16 media under `build/output` is already consumed through
the original `-f/-w/-a` activation. The fixed container observes original
`50:11`, `50:3B`, `50:0F`, `50:1B`, and `54:05`, then exits `0xC0000005`
before the next BOP ingress. This is not a missing guest loader or failed BOP
provider; see [T335 startup observation](../evidence/m0-t335-s3-p11-original-wow-startup-activation-observation-001.md).

## Scope

- Freeze the same normal console-owning container and selected media.
- Walk source and guest continuation from `cmdSetInfo` into NTDOS `msinit.asm`,
  including CCPU40 frame/segment/stack state, SAS memory, BIOS exchange and
  any actually reached FDC/INT15/heartbeat/PIT/ICA predecessor.
- Recover only by original source, same-shaped binding or registered minimal
  overlay. Do not use the trace to choose a BOP service change.
- Preserve load-only guest media and original NTDOS/COMMAND ownership; use the
  existing session mapping manager for guest spans.
- Require paired x86/x64 formal compile/link evidence. Runtime is claimed only
  for an architecture that was actually observed.

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

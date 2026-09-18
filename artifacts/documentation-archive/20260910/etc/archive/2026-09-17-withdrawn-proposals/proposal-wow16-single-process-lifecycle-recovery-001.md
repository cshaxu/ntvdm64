# Withdrawn — WOW16 `WRITE.EXE` workload recovery

## Disposition

Withdrawn from the global T queue on 2026-09-18. Its scope duplicated the
active T420 runtime-package program: [T420 S41 WOW16 capability closure]
(../../../../../../docs/proposals/proposal-mvdm-runtime-package-completion-001.md#s40--s43-capability-closure-requirements)
now owns the complete WOW16 loader, task, callback-return and `WRITE.EXE`
workload contract. This retained record preserves prior historical links; it
does not admit a second WOW16 implementation package.

## Objective

Superseded by T420 S41. The immediate original owner that first prevents the
selected `WRITE.EXE` Win16 workload from starting, running and returning is
identified and recovered only within S41's complete WOW16 package scope.

## Boundary

The successor retains the same boundary: `run16.exe`, `basesrv.exe` and the
worker remain separate processes, and broker availability cannot be used to
bypass an incomplete guest contract.

Preserve the source-owned low-memory dialog and guest error route. Do not add a
private USER/CSRSS shell, broker transport, CPU tracing policy, or substitute
guest/UI failure behavior.

## Exit evidence

No independent exit evidence remains. S41 carries the former requirements as
mandatory package acceptance, including first-blocker attribution and the
source-first dependency-promotion rule.

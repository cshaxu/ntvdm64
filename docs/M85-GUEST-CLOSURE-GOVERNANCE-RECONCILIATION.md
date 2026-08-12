# M85 Guest Closure Governance Reconciliation

Status: completed documentation reconciliation, 2026-08-08.

## Question

The full OpenNT MVDM descriptor census has 75 declared rows, while the fixed
NT4 DOS guest install closure has 33 files. This record checks whether current
planning material still treats retired JWasm/UASM/WLink/OpenWatcom experiments
as active blockers, or mistakes the 75-row system census for 75 outstanding
DOS guest builds.

## Current Evidence

The current authority, `HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`, proves
the direct closure as 22 unchanged-source OpenNT-tools16 builds, seven static
companions, and four explicitly non-source-built OpenNT `binplace` inputs.
`NT4-GUEST-ARTIFACT-CLOSURE.md` names every member. The five separately scoped
WOW16 device drivers are also source-built, but are not a reconstructed WOW
runtime.

`OPENNT-MVDM-DECLARED-OUTPUT-TRIAGE.md` accounts for every descriptor row:
direct DOS payload, host/support, tool, sample/development-only, utility/debug,
or WOW dependency. Therefore the 75-row figure is not an uncompleted DOS-file
count and must not be used to claim either guest incompleteness or a completed
whole-system NTVDM rebuild.

## Changes

The following active planning records now name the current historical tools16
result rather than earlier experimental linker failure:

- `OPENNT-ARTIFACT-MATRIX.md`: command, DOSX, MEM, NLSFUNC, LOADFIX,
  GRAPHICS, devices, utilities, and `MOUSE.DRV` use current source-build
  state; runtime admission remains separately pending.
- `OPENNT-FULL-SET-BUILD-CAMPAIGN.md`: direct batches no longer appear as the
  next source-build task; the next payload proof is the governed P0 trace.
- `OPENNT-FULL-ARTIFACT-CLOSURE-WORK-ORDER.md`: COMMAND's obsolete linker
  recovery task is retired, and P1 is explicitly runtime admission work.

Older build-probe reports remain historical diagnostic evidence and were not
rewritten or deleted. This reconciliation does not change their provenance;
it prevents them from governing the next implementation decision.

## Remaining Work

1. The four `binplace`-only files have no fixed-tree source-build rule and
   remain transparent external inputs, not failed builds.
2. The direct closure still needs a governed backend and source-backed live
   trace before any file is called loadable or runnable.
3. The host/support and broader WOW rows are classified but not reconstructed
   as final products. They are admitted only when an actual bounded profile
   trace proves a need.

This task does not relax M70/M79 or alter any `ntvdm64` source.

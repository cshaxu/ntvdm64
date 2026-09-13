# M0 T407 S3 DPMI mode-frame restoration

## Decision

S3 rechecked D35 against the original owner
`O:\repos.external\OpenNT\base\mvdm\dpmi32\i386\dpmi386.c`. Both current
CPU40 receivers already preserve the original DOSX frame layouts:

- protected entry reads `DS` at +0, `ESP` +2, `SS` +6, `EIP` +8 and `CS`
  +12 from the current stack;
- BOP FD real-mode return reads `DS`, `SP`, `SS`, `IP`, `CS` at +0, +2, +4,
  +6, +8 respectively.

The original i386 code writes passive kernel-VDM `CONTEXT` fields before its
mode-bit update. CPU40 setters immediately load descriptor caches, so that
order is not composable: a protected selector cannot be loaded while the
emulator still treats it as real-mode, and a real-mode segment cannot be
loaded while PE is still set. The retained seam captures the original frame,
changes PE at the active-CCPU boundary, then loads the same values in the mode
in which their caches are valid. It neither changes frame values nor imports
the original fixed NTVDM-state-page RI/RM writes, which have no standalone
carrier.

The only removable D35-local divergence was
`mvdm_softpc_record_dosx_real_mode_frame`: an optional host-file reporting
call between frame capture and PE clear. Search found this to be its sole
caller. It was removed along with its declaration/body, restoring a direct
capture → PE-clear → active-cache-load path without guest-state, BOP, I/O or
logging side effects.

## Source-recovery ledger

| Rung | Result | Disposition |
| --- | --- | --- |
| 1. Original source reuse | Used | Original `switch_to_protected_mode` and `switch_to_real_mode` own every field offset and visible value. The local mirror preserves them. |
| 2. Smallest same-shaped facade | Used | CPU40’s active segment setters provide the necessary cache-order seam, limited to PE/CPL and segment-cache realization after original-value capture. |
| 3. External-code intrusion | Rejected | No external source or kernel source was altered/imported. |
| 4. New behavior | Rejected | The non-original frame-recording operation was removed; no frame algorithm was added. |

## Explicit exclusions

The surrounding native TSS carrier, IDT source restoration, GDT/LDT shadow,
FastWOW TEB projection and protected stacks are U04, not D35. They remain
unchanged and are not evidence that this S accepts their semantics. S4 owns
their source/consumer audit only and must return for design approval before
implementation.

## Verification

| Check | Procedure | Result |
| --- | --- | --- |
| Paired frame review | Read original `dpmi386.c`, current `modesw.c`, the 486 DOSX FD producer and BIOS FD table selection. | Field offsets and original continuation ownership agree; only active-cache order differs, for the stated CPU40 reason. |
| Sole-call removal sweep | Repository search for `mvdm_softpc_record_dosx_real_mode_frame`. | Before edit: one call in `modesw.c`, one declaration and one body. After edit: no remaining reference. |
| Historical functional evidence | [T395 history](../../history/m0-t395-consolidated-history.md) and the T404 FD contract audit record the same CPU40 `53:01` and FD paths reaching downstream original DPMI services/nonzero continuations. | Supports the retained seam; it is not a new WRITE acceptance claim. |
| Formal integration | Explicit x86 Ninja target `original-softpc-process.exe` under `build/M0-T407/S2/r002-formal-x86`. | Recompiled changed `modesw.c` and termination adapter, then linked successfully. Existing compiler warnings remain non-fatal. |
| Regression fixtures | Existing XMS initialization and mapped-memory fixtures. | Both exit `0`; they are build-regression evidence only, not D35 behavioral proof. |

No new runtime guest observation was created because the only behavior change
is removal of an optional host report. The previous D35 functional evidence
is retained rather than falsely presenting a source-only cleanup as a new
DOSX/WOW run.

## Follow-up

S4 will audit U03 cache-choice and U04 platform projections, enumerate each
original owner/consumer/reset/teardown rule, calculate removability, and stop
for owner-approved design before changing source.

# M0 T310 S8 P4 current full-tree x86/x64 baseline

## Scope

This is a reproducible compiler baseline only for the admitted native-width
audit scope:

- `src/mvdm-host/**`
- `src/mvdm-support/**`

It does not claim that every compiler warning is an ABI defect, that every
original package is runtime-complete, or that S8 is closed.  Other component
trees were not modified or used as the audit scope.

## Procedure

Starting with cleaned target-local Ninja trees, the formal MSVC `/MT` Ninja
graphs were run independently:

- `build/M0-T310/S8/p1-machine-source/x64`
- `build/M0-T310/S8/p1-machine-source/x86`

The complete logs are retained as disposable target-local build outputs:

- `x64/s8-p4-current-full-baseline.log`
- `x86/s8-p4-current-full-baseline.log`

Each run reached `[357/357]` and reported zero compiler/linker errors. The
same parser used for the governed compiler-warning ledger recorded 40,059 raw
diagnostic occurrences, yielding 19,466 current unique source diagnostics.
Of those, 1,035 remain in the x86/x64 source-review worklist. All diagnostics
remain deliberately visible rather than being hidden by warning suppression.

## Interpretation

The two targets now establish one current selected-source baseline for P4:
the same 357-step original SoftPC/MVDM composition graph compiles completely
on x86 and x64.  The near-equal warning populations are an audit input, not a
parity conclusion: a warning may be historical source style, guest-width
arithmetic, a genuine native-width issue, or a call-contract defect.

The classified ledger is reconciled by exact current source identity
(`path`, `line`, warning, message). Resolved diagnostics disappear. A warning
that still occurs is re-opened for current source review rather than inheriting
an earlier `closed-*` disposition. Only enduring non-width or hard-boundary
decisions may carry forward when that exact identity still exists.

Subsequent P4 work must read each complete original contract cluster
(declaration, definition, table/initializer and reached call sites) before a
mirror, overlay, adapter, mapping-manager or unavailable disposition is made.
No warning may be suppressed merely to alter this baseline.

## Follow-up

Use the existing whole-tree static ABI inventory, compiler-warning ledger,
classification worklist and cluster ledger to select complete original owner
packages.  The next source work is the pending CCPU/CVIDC/base-video and
host-controller contract clusters; it is not a license to repair one warning
solely because it appears first in a build log.

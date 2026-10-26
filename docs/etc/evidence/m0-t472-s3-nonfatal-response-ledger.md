# M0 T472 S3 — Nonfatal error-response ledger

## Question and method

This source-ledger verifies that each selected nonfatal response remains owned
by its original immediate caller.  It compares the reached response code with
the pinned OpenNT mirror and deliberately does not install a generic response
broker, callback, or replacement dialog.

## Concrete response families

| Family | Offered buttons | Original caller behavior | Current disposition |
| --- | --- | --- | --- |
| PIF start-directory/file/extension/tail (`dos/command/cmdpif.c`) | Abort, or Edit for the `?` command line | All Abort paths go to `CleanUpAndReturn`; Edit changes only `pfdata.CmdLine`, whose nonempty value is then processed by the original PIF path. | Retained.  The only local diff is the previously accepted session-safe write of `IsDosBinary`; it is after the selected dialog decisions. |
| Floppy sharing violation (`nt_rflop.c`) | Abort / Retry / Ignore | `get_drive_handle` repeats only when `DiskOpenRetry(...) == RMB_RETRY`; Abort and Ignore break to the existing invalid-handle result. | Retained.  Its local source differences are guest-location/lease compatibility, not result handling. |
| PIF allocation (`nt_pif.c!ch_malloc`) | Abort / Retry / Ignore | Retry repeats `malloc`; Ignore breaks and returns `NULL`; the immediate caller owns the allocation failure. | Retained.  System-path and lifetime differences are separate approved firmware/configuration seams. |
| DPMI fault (`dpmiint.c`) | `RcErrorDialogBox` Abort / Ignore UI | After dialog return, original code restores the documented exception frame (`CS:EIP:SP/ESP`) and continues; this is caller-local exception recovery. | Retained. |
| DPMI virtual-memory failure (`dpmiint.c`) | Error dialog then terminal result | The current selected original branch explicitly calls `TerminateVDM` after the dialog. | Covered by S2’s terminal owner; no false nonfatal claim. |

## `host_error` label boundary

`host_error(error_num, options, extra)` has a historical peculiarity: it
always invokes `ErrorDialogBox` with Abort/Ignore, ignores `options`, and
always returns `ERR_CONT`.  Therefore neither displayed button establishes a
global continuation contract.  The actual caller decides whether that return
continues, fails a local operation, resets, or reaches another terminal path.

This is not repaired here.  Changing labels or suppressing an option would be
a product-policy choice, not original recovery.  The finite S3 result is that
the current code preserves this literal source behavior and no adapter
silently turns it into a global retry/ignore engine.

## Mirror and verification boundary

The selected control-flow blocks in `cmdpif.c`, `nt_pif.c`, `nt_rflop.c`, and
`dpmiint.c` retain their original response interpretation.  Their current
mirror differences concern previously approved guest-location, immutable
system-root, configuration-path, or CCPU memory publication adaptations;
none consumes or rewrites `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, or
`RMB_EDIT`.

Automating modal choices would require a test-only dialog control seam that
does not exist in the product contract.  S3 therefore records source-proven
dispositions rather than fabricating a dialog response.  S5 may add a real
modal integration witness only if it can drive the original dialog without
introducing a second dialog owner.

## Result

No production change is justified.  The next finite concern is S4’s already
separated WOW hard-error path: confirm its exact response slots and the
original `W32HungAppNotifyThread` task/process handoff against the existing
T464 boundary, without expanding private CSRSS/USER emulation.

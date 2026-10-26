# M0 T472 S4 — WOW hard-error reconciliation

## Scope and conclusion

This stage rechecks the selected WOW hard-error path against the already accepted T464 delivery. It makes no source, ABI, guest, package or release change. The current path has one decoder/presenter and one terminal owner: the project-owned public boundary preserves the original response source slot; the original WOW caller tail owns any task or worker consequence.

## Current route and authority

```text
original WOW32 caller
  -> WOWpSysErrorBox (exact STATUS_VDM_HARD_ERROR request)
  -> ADAPTER-WIN32-040 public receiver
  -> mvdm_wow_hard_error_present / source-slot decoder
  -> ErrorDialogBoxThread
  -> original W32HungAppNotifyThread / original caller tail
```

`src/ntvdm-exe/win32/wow_private_unavailable.c` accepts only the exact current-session hard-error shape. It sends that request to `mvdm_wow_hard_error_present`; unknown private request shapes cause the controlled unavailable-path stop (`ERROR_CALL_NOT_IMPLEMENTED`) rather than a fabricated response or a reconstruction of private CSRSS/USER state.

`src/mvdm/softpc.new/host/src/nt_error.c` still routes a WOW Abort selection to the loaded original `pW32HungAppNotifyThread(0)`. The retained original WOW implementation owns task retirement, hung-task handling and any whole-worker result. NTSRV/worker lifecycle code only authorizes the resulting worker exit; it is not a second hard-error terminator.

The decoder keeps original **source slots**, not compacted visible-button ordinals. Its accepted mapping preserves a later valid default and the actual Cancel slot. Thus the adapter cannot turn a response for one original button into a response for another.

## Evidence and bounded verification

T464's retained executable fixture `tests/observation/verify-wow-hard-error-decoder.ps1` passed the five source-slot/default/cancel assertions and its Full gate passed fourteen product gates. The published evidence is [T464 S4 hard-error acceptance](m0-t464-s4-hard-error-acceptance.md).

For this source-only reconciliation, an attempted fresh fixture launch did not create an owned result receipt. It is **not counted as a pass or fail**; the user explicitly directed that this unavailable verification not block continued work. Current source inspection confirms no changes to the decoder, public receiver, original WOW handoff, or packaged runtime images since the retained T464 acceptance. Consequently S4 relies on the retained executable proof plus current source identity rather than inventing a modal driver or claiming a new runtime witness.

## Retained boundary

The private NT4 `DisplayVDMHardError` server remains unavailable on the modern host. This stage does not claim private modal focus, labels, title bars or an automated click. That is a deliberate bounded non-pass, not a missing second termination implementation. No new issue transfers from S4 to S5.

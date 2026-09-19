# Proposal — OpenNT error-dialog response and termination semantics recovery

## Objective

Recover and prove the selected OpenNT error-dialog contract for the standalone
worker.  A visible button must have one source-defined outcome: terminate the
affected DOS/VDM execution, retry its documented operation, or return control
to the caller's documented recovery path.  In particular, a user selecting
**Terminate** must not leave a flashing Console, a live task record, or a
worker blocked before it notifies BaseSrv.

This package begins only after the active T420 S34 packet closes.  It is a
behavioral recovery/verification package, not a reason to redesign COMMAND,
BaseSrv records, CCPU execution, guest media, or the global broker lifecycle.

## Source findings and boundary

The selected OpenNT NT host has three distinct mechanisms:

| Mechanism | Original response rule | Required disposition |
| --- | --- | --- |
| `ErrorDialogBox` / `RcErrorDialogBox` | `Terminate` maps to `RMB_ABORT`; DOS calls `TerminateVDM`; `Retry` and `Ignore` return to the immediate caller. | Preserve the original result mapping and prove the complete standalone termination tail. |
| `RcMessageBox` | Returns `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, or `RMB_EDIT`; its caller alone owns the resulting control flow. | Preserve caller-local retry, abandonment and edit paths; do not invent global Ignore semantics. |
| `host_error` | The selected original `nt_error.c` presents Abort/Ignore but ignores its `options` parameter and returns `ERR_CONT`. | Keep this literal source behavior during recovery; identify every reached caller whose displayed Ignore is not a viable recovery path. |

The original DOS terminal tail is:

```text
TerminateVDM -> terminate -> host_applClose -> host_terminate
             -> ExitVDM -> worker completion/receipt -> process exit
```

The preceding standalone diagnosis found an adapter-added alert-and-infinite
join before that tail.  It is not original OpenNT behavior and can keep the
worker alive after a fatal dialog response.  This candidate must establish the
current formal source/build selection and retain only the original alert
ordering required for the terminal path.

WOW is a separate original contract: an Abort result is delivered through
`WOW32.DLL!W32HungAppNotifyThread`, not a direct DOS `TerminateVDM` call.  Its
public modern presentation seam may translate only the existing hard-error
request and selected button response; it must not fabricate a response or
substitute a new USER/CSRSS task manager.

## S tasks

| S | Scope | Exit condition |
| --- | --- | --- |
| S1 | Freeze a source/caller ledger for every selected `ErrorDialogBox`, `RcErrorDialogBox`, `RcMessageBox`, `host_error`, `RcErrorBoxPrintf`, `DisplayErrorTerm` and WOW hard-error route.  Classify all offered buttons and the immediate post-return path as terminate, retry, caller-local continuation, or unavailable. | No reached dialog route lacks a source owner, button mapping, or post-response disposition.  Original and current mirror differences are separately recorded. |
| S2 | Restore the DOS fatal dialog terminal path to original ordering and remove any standalone-owned wait/join or process-lifecycle behavior that can block `ExitVDM` completion.  Add only focused test instrumentation outside mirrors when source-level observation is insufficient. | A missing/invalid selected system file and one command/environment fatal path each prove: dialog -> Terminate -> worker receipt -> worker process exit -> parent completion, with no Console leak. |
| S3 | Prove all meaningful nonfatal response families: `RcMessageBox` retry/ignore/edit, VDD failure continuation, disk retry/abandonment, and DPMI fault continuation.  Where a `host_error` caller unconditionally terminates after return, record the literal source behavior and decide whether a truthful one-button product presentation is separately admissible. | Each offered nonfatal button has a real or source-proven unavailable witness; no UI label is claimed to have a global meaning it does not possess. |
| S4 | Recover/prove the selected WOW hard-error response path: response encoding, button mapping, `W32HungAppNotifyThread` availability and task/process cleanup.  Keep unavailable private CSRSS/USER facilities outside the worker. | A reachable WOW path either proves Abort ends the target task through original WOW ownership or records the exact unavailable owner/boundary; non-WOW regressions remain clean. |
| S5 | Perform full response/lifecycle regression and final mirror/adapter audit.  Remove test-only observation from production paths, update all affected boundary ledgers, and report source-delta versus product UI-policy delta separately. | Formal x86 build, direct/nested COMMAND/MEM/EDIT regressions, fatal completion matrix, response matrix, governance and diff checks pass; every retained deviation has a named owner and removal condition. |

## Product policy decision point

Literal source recovery does **not** make every original label good product UX.
For example, numerous original fatal callers show Ignore and then immediately
call `TerminateVDM`.  After S1 establishes the complete caller ledger, a
separate explicit product-policy decision may replace those specific dialogs
with a truthful Terminate-only presentation.  That is a documented modern UI
change, not an alleged OpenNT restoration, and it may not change retry/ignore
semantics for calls that genuinely use them.

## Non-goals and stop conditions

- No generic global error dispatcher, generic Continue behavior, alternate
  command parser, synthetic guest completion, or broad dialog rewrite.
- No restoration of CSRSS, CSR transport, private USER/GDI services, kernel
  VDM or a second task-management implementation.
- No assumption that a dialog closing proves termination; worker, broker
  receipt and parent completion must be observed.
- Pause for a new admission decision if evidence requires changing guest
  media, the broker protocol, a CCPU execution semantic, or a non-selected
  WOW/USER package.

## Required evidence

Record the pinned source paths/hashes, exact build selection, each dialog
request and button, post-response state, worker PID/receipt status, parent
completion and Console ownership.  Failed/unavailable WOW or private-server
paths are evidence, not passes.  Preserve existing direct and nested
`COMMAND`, `MEM` and `EDIT` behavior throughout.

# M0 T472 S1 — Error-dialog owner ledger

## Scope and method

This is a source-only ledger.  It traces the selected error-dialog family
through the current x86 source and the pinned OpenNT mirror; it does not claim
runtime acceptance.  Guest media was neither read as an execution dependency
nor modified.

| Mirror | OpenNT source | Current source | Difference |
| --- | --- | --- | --- |
| `nt_error.c` | `3305EAABAC76E039DFE80AA0F2E15C48433EDD95A74615E5DB55B97D2B393228` | `09E1F4D221DB21E04A32096380C4E48EDEC23863943A068DA7118B1E542D6957` | Public callback/calling-convention repairs only; no button or control-flow change. |
| `nt_reset.c` | `9AA8120237FFA104C87318EF39747A0E66C28BF56BFE9AF03511BDDF13F11B47` | `857F1EC8DC743CFC77BBE6BCF3ACC1FC271950B6F86CA8D81E65B3B68DCE5739` | Earlier lifecycle authorization and source-owned close-cohort divergences. |
| `wkman.c` | `56BE1989F2BC3ACAFFCFA6345B20E38F5E7312BADF03AF04D4565F8F4A489AC9` | `F66805A02FD6D29A7AA1001BA2E549507B66F1A96E2BE3460090C007B0D472A2` | Earlier idle/task-management/lifecycle divergences; selected hard-error termination remains original-owned. |

## Source-owned response contracts

| Entry | Offered response and direct result | Immediate owner after return | Disposition |
| --- | --- | --- | --- |
| `ErrorDialogBox` | `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, optionally `RMB_EDIT` | DOS `RMB_ABORT` calls `TerminateVDM`; Retry/Ignore/Edit return to its caller. | Original algorithm retained. |
| `RcErrorDialogBox` | Always supplies Abort/Ignore to `ErrorDialogBox`; it has no result value. | Its caller owns subsequent action.  Fatal callers explicitly call `TerminateVDM` or return to a source-defined failure path. | No global “Ignore means continue” rule. |
| `RcMessageBox` | Returns exact `RMB_ABORT`/`RMB_RETRY`/`RMB_IGNORE`/`RMB_EDIT`; Edit copies edited text back to `msg2`. | Immediate caller alone interprets it. | Original algorithm retained. |
| `RcErrorBoxPrintf` | Supplies Abort/Ignore and returns `void`. | Caller continues according to its pre-existing control flow. | Original algorithm retained. |
| `DisplayErrorTerm` | Formats a system error, calls `host_error(..., ERR_QUIT, ...)`, and returns its value. | `host_error` literally returns `ERR_CONT`, regardless of its ignored `options`. | Historical literal behavior; not evidence of a global Recover action. |
| `host_error` | Displays Abort/Ignore but unconditionally returns `ERR_CONT`. | Every caller owns its own continuation/termination. | Historical literal behavior; apparent Ignore can be non-viable at a specific caller. |

The common DOS Abort tail is source-owned and unchanged:

```text
ErrorDialogBox -> TerminateVDM -> terminate -> host_applClose
  -> host_terminate -> ExitVDM -> worker receipt/process exit
```

`host_applClose`/`host_terminate` now call the existing
`MVDM-HOST-DIV-356` lifecycle authorization before their original cleanup.
That bridge does not choose a dialog result, alter the original resource
release order, or create a second termination route; it lets the central
worker lifecycle observe the source-selected exit.

## Reached caller families

| Caller family | Selected path | Post-response owner | Classification |
| --- | --- | --- | --- |
| COMMAND/PIF validation (`cmdpif.c`) | Start-directory/file/extension/tail errors call `RcMessageBox(..., RMB_ABORT)` then `CleanUpAndReturn`. | COMMAND caller; Abort is a failed launch, not VDM-wide death. | Caller-local abandonment. |
| COMMAND PIF prompt (`cmdpif.c`) | `RMB_EDIT` updates `pfdata.CmdLine`; empty text is retained as a source-defined no-command result. | COMMAND caller. | Caller-local edit. |
| floppy lock (`nt_rflop.c`) | `DiskOpenRetry` returns Abort/Retry/Ignore; `get_drive_handle` retries only on `RMB_RETRY`, otherwise returns failure. | Floppy caller. | Real retry versus caller-local abandonment. |
| PIF allocation (`nt_pif.c`) | `ch_malloc` retries allocation; Ignore breaks and returns `NULL`. | PIF caller. | Real retry/ignore continuation. |
| DPMI (`dpmiint.c`) | Bad-fault route displays `RcErrorDialogBox` then follows its documented exception-frame handling; allocation failure displays then explicitly calls `TerminateVDM`. | DPMI caller. | Source-defined continuation / explicit fatal tail. |
| COMMAND/DEM/config startup | Resource dialogs in `cmdconf.c`, `cmdmisc.c`, `cmdredir.c`, and `demmisc.c` return to explicit startup-error cleanup or fatal paths. | Immediate startup caller. | Caller-local failure; no global response inference. |
| SoftPC host/device paths | `DisplayErrorTerm` reaches `host_error`; `host_error` return remains the historical `ERR_CONT`, and individual device/startup callers decide whether continuing is safe. | Immediate SoftPC caller. | Literal historical contract; product-label review belongs to S3. |

## WOW hard-error and task termination

For a WOW worker, `ErrorDialogBoxThread` calls the original
`WowErrorDialogEvents`, which encodes selected buttons into the original
`WOWpSysErrorBox` four-parameter hard-error request.  A mapped Abort returns
`RMB_ABORT` and invokes the original `WOW32.DLL!W32HungAppNotifyThread(0)`;
that routine owns active-task termination and only escalates to whole-WOW
termination under its original hung-task policy.

The modern public-API boundary is deliberately narrow:

* `ADAPTER-WIN32-040` recognizes exactly that four-parameter VDM hard-error
  shape, presents a local response, and returns the original response slot.
* Any other private hard-error shape is an explicit unavailable boundary:
  controlled current-session stop with `ERROR_CALL_NOT_IMPLEMENTED`, never a
  fabricated response or an emulated CSRSS/USER provider.
* `MVDM-HOST-DIV-449` only binds/retires the existing management ticket around
  original task creation/destruction; it does not replace
  `W32HungAppNotifyThread`.

## S1 conclusion and S2 boundary

Every selected entry has a source owner, response mapping, and post-response
disposition.  The mirror shows no current adapter-added alert, join, or
second terminator in the selected `nt_error.c` route.  Therefore S2 is not
authorized to redesign the dialog path.  Its finite work is to prove, with
one connected DOS-fatal source batch, that the existing lifecycle bridge
preserves the shown original Abort ordering through broker receipt and process
exit; any source change must be limited to a demonstrated ordering defect.

Uncovered here: runtime dialogs requiring interactive input, private hard
errors outside the exact recognized shape, and product wording where a
historical Ignore label cannot actually continue.  Those remain S2/S3/S4
verification/policy boundaries, not unowned source routes.

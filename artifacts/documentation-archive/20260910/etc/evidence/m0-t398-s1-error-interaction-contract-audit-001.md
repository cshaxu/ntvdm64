# M0 T398 S1 — app-owned error-interaction contract audit

## Scope and result

This is a source/ABI audit only. It selects no UI implementation, does not
alter a product dialog, and does not treat a dialog capture as application
lifecycle success. The only product/acceptance target is Win32/x86
`ntvdm32.exe`.

The reached product error surfaces separate into three ownership families:

| Family | Current owner and source | Result/action contract | S1 disposition |
| --- | --- | --- | --- |
| Guest application prompt | Win16 application source and public WOW message route; the observed WRITE `#32770` is `diaalert.c:Error → IdPromptBoxSz → MessageBox` | The guest owns the message, MB style, parent disabling, selected button, and any later guest state change. `Error()` itself deliberately discards the ordinary button result unless its disabled `CANCELMSG` build switch is selected. | Deferred. It is not a host/SoftPC error and must not be re-presented or given app termination policy. |
| SoftPC/NTDOS resource error | `softpc.new/host/src/nt_error.c`: `RcErrorDialogBox`, `RcMessageBox`, `host_error`, `host_direct_access_error`, `RcErrorBoxPrintf`, and the `ERRORPANEL` resource | The caller supplies an error resource plus optional copied OEM strings and `RMB_*` options. `RcMessageBox` returns `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, or `RMB_EDIT`; the edit form copies the edited OEM text back to caller storage. `RcErrorDialogBox` has no result. | First implementation cohort. The existing original carrier has a finite, public-Win32-representable request/reply protocol. |
| App product-preflight error | `app/entry.c:app_report_media_root_rejected` | One app-owned `MB_OK | MB_ICONERROR` message, followed by `APP_STARTUP_MEDIA_REJECTED`. | Must join the future app-owned presentation owner, but is not part of the first adapter migration: it already has app-owned completion and no original `RMB` reply contract. |

## Original SoftPC contract

`error.h` defines the only reply ABI:

```text
RMB_ABORT = 1, RMB_RETRY = 2, RMB_IGNORE = 4, RMB_EDIT = 128
```

The low option word also selects information, warning, stop, or question
icon; the high word is the edit-buffer capacity. `ErrorDialogEvents` maps
Terminate, Retry, Ignore/Cancel, and edit-OK to those reply values exactly.
It hides unselected controls rather than assigning an implicit replacement
default.

`ErrorDialogBox` currently makes two decisions that a generic presentation
surface must not absorb:

1. It derives the error title from the originating Console/WOW window and
   serializes recursive requests with `bCalled`; and
2. after an `RMB_ABORT`, it invokes the original WOW hung-app notification or
   `TerminateVDM` for a DOS VDM. A thread-creation failure has the same
DOS-terminate/WOW-ignore asymmetry.

Thus a future presenter may return a copied reply, but only `session` may
turn the selected terminating reply into current-session completion. The
presentation must not call `TerminateVDM`, invoke the WOW callback, retain an
origin HWND, retain `pEdit`, or encode a new default response.

`host_error()` is intentionally not a reply-forwarding caller: it always
returns `ERR_CONT` after showing an abort/ignore panel. `RcErrorDialogBox`
also intentionally discards the reply. By contrast, `RcMessageBox` is a
reply-forwarding family: PIF command-line and floppy callers branch on the
returned `RMB_*` value, so it cannot be reduced to an OK-only error.

## Private and terminal boundaries

`WowErrorDialogEvents → WOWpSysErrorBox` is an NT4 private hard-error path
with its separate `SEB_*` ordinal protocol. The current source deliberately
uses the original `ERRORPANEL` handler instead; no private hard-error service
is selected.

The NTDOS/COMMAND callers of `RcErrorDialogBox` frequently call
`TerminateVDM` immediately after it returns. That is caller-owned source
semantics, not a presentation default. The same is true of product-preflight
return statuses in `app/entry.c`. Neither may be collapsed into one global
"close means terminate" rule.

The current WRITE low-memory dialog stays guest-owned. Its current ordinary
capture proves only the first application error boundary from T397; it does
not select the guest dialog as an app error carrier.

## First S2 cohort

S2 may design and implement only a copied request/reply boundary for the
`ERRORPANEL` carrier:

- source adapter input: bounded ANSI/OEM message, optional bounded edit text,
  icon/options and origin-title snapshot;
- app input: copied, native-width-free public data with the exact selectable
  `RMB_*` set and no guest pointer/HWND ownership;
- app output: one explicit `RMB_*` reply plus optional copied edit data; and
- session input: only an explicitly selected terminating result, preserving
  the original caller's DOS/WOW distinction rather than creating a UI-owned
  termination policy.

S2 must include exact button/default/result fixtures, no-repeat/error-thread
behavior review, and an ordinary x86 regression showing that the existing
WRITE guest error remains guest-owned. It may not migrate `RcMessageBox` edit
semantics, app-preflight presentation, `WOWpSysErrorBox`, or guest `MessageBox`
families until their separate admitted cohorts.

## Source locations reviewed

- `src/mvdm-host/softpc.new/host/inc/error.h`
- `src/mvdm-host/softpc.new/host/src/nt_error.c`
- `src/mvdm-host/softpc.new/obj.vdm/resource.rc`
- `src/mvdm-host/dos/command/{cmdconf.c,cmdenv.c,cmdmisc.c,cmdpif.c,cmdredir.c}`
- `src/mvdm-host/dpmi32/dpmiint.c`
- `src/app/entry.c`
- `src/session/{session.c,session_termination.c}`
- `src/mvdm-guest/wow16/write/diaalert.c` (classification only; no guest
  source/media change is selected)

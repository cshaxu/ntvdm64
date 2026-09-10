# M0 T396 S10 — error-dialog family map 001

## Question

Which reached error interactions already have a visible public-Win32 route,
which are diverted through the unavailable private WOW service, and what
source-defined result domain must each retained route preserve?

## Inputs

- `src/mvdm-host/softpc.new/host/src/nt_error.c`
- `src/mvdm-host/softpc.new/obj.vdm/resource.rc`
- `src/mvdm-host/wow32/{wudlg.c,wow32.c,wuser.c,wkman.c,wumsg.c}`
- `src/mvdm-host/inc/softpc.h`
- current `adapter-mvdm-host-out/win32/source/wow_private_unavailable.c`
- S9's reached private-hard-error contract audit

## Family disposition

| Family | Original source path | Visible public route today | WOW/private divergence | Result contract | S10 disposition |
| --- | --- | --- | --- | --- | --- |
| Standard MVDM resource error | `nt_error.c:RcErrorDialogBox` → `ErrorDialogBox` | Original `ERRORPANEL` resource and `ErrorDialogEvents` | None on non-WOW route | `RMB_ABORT` / `RMB_RETRY` / `RMB_IGNORE`; optional `RMB_EDIT` | Retain direct original route. |
| Standard error while `VDMForWOW` | `nt_error.c:ErrorDialogBoxThread` | Same original `ERRORPANEL` source/resource exists | The WOW branch calls `WowErrorDialogEvents` rather than `DialogBoxParam`, then enters private hard error | Same `RMB_*` domain; abort retains original WOW hung-task termination handling | First implementation target: reuse the original `ERRORPANEL` route, preserving the existing post-result handling. |
| WOW host `WOWSysErrorBox` | `nt_error.c:WOWpSysErrorBox` / `WOWSysErrorBox` | None after its private transport | Exactly one original `NtRaiseHardError` call | Raw selected ordinal `1` / `2` / `3`; source currently maps non-success to `0` | Needs a narrow private-hard-error facade with copied title/message and source-defined button descriptors. |
| Win16 USER/KERNEL `SYSERRORBOX` | `wow32/wudlg.c:WU32SysErrorBox` and `wow32.c:W32Exception` | Delegates to `WOWSysErrorBox` | Inherits the same private transport | Its supplied `SEB_*` descriptors map directly to ordinal `1` / `2` / `3` | Covered by the `WOWSysErrorBox` facade; no separate dialog implementation. |
| WOW event-hook error | `wow32/wuser.c:WU32SetEventHook` | Delegates to `WOWSysErrorBox` | Inherits the same private transport | Only result `3` preserves the task; all other results force task exit | Covered by the same facade; fixture must include Ignore and non-Ignore. |
| WOW standard `MESSAGEBOX` | `wow32/wudlg.c:WU32MessageBox` | Direct public `MessageBox` | None | Native source-selected `MessageBox` result IDs | Retain unchanged; it is not a hard-error ABI. |
| WOW host diagnostics | `wow32/{wkman.c,wumsg.c,wow32.c}` | Direct public `MessageBox` | None | Per-call source-selected standard IDs, often `IDOK` | Retain unchanged; audit each reached caller before changing it. |
| Other direct MVDM UI | `softpc.new/host/src/{nt_smenu.c,...}` | Direct `DialogBox`/`MessageBox` | None found by this sweep | Per-call public Win32 result | Retain unchanged; not folded into `SYSERRORBOX`. |

## Required descriptor semantics

`softpc.h` and the original host `error.h` define the `SYSERRORBOX` button
vocabulary as `SEB_OK=1`, `SEB_CANCEL=2`, `SEB_YES=3`, `SEB_NO=4`,
`SEB_RETRY=5`, `SEB_ABORT=6`, `SEB_IGNORE=7`, and `SEB_CLOSE=8`; bit
`SEB_DEFBUTTON=0x8000` selects the default.  The returned result is its
**position** (first/second/third present button), not the numeric `SEB_*`
value.  The S10 facade must keep that distinction, permit zero (absent)
descriptors, and reject malformed descriptors instead of selecting a button.

`ERRORPANEL` has a separate `RMB_*` contract.  Its resource labels are
Terminate, Retry, Ignore and optional edit/OK; its original event procedure
returns `RMB_ABORT`, `RMB_RETRY`, `RMB_IGNORE`, or `RMB_EDIT`.  It is not a
valid implementation of all `SEB_*` combinations, although its direct reuse
is the first recovery rung for the `ErrorDialogBoxThread` WOW branch.

## Interpretation

The owner-selected “same visible-dialog mode” does not authorize a generic
replacement API.  It means each existing source family remains visibly
interactive through modern public Win32, while retaining its own layout,
button labels/default and result mapping.  Existing direct `MessageBox` and
`ERRORPANEL` routes remain source-owned.  Only the two private-WOW families
need recovery work.

## Follow-up

Implement the `ERRORPANEL` direct-reuse branch first, then introduce the
smallest dedicated `SEB_*` dialog facade only for the exact S9 hard-error
shape.  Positive tests must cover each returned ordinal and default-button
selection; negative tests must cover unsupported hard-error shape, malformed
descriptor, absent session and no pointer escape.

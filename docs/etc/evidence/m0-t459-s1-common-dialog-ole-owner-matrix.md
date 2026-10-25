# M0 T459 S1 — common-dialog and OLE owner matrix

**Status:** closed source/boundary audit. This record separates the two W18
families before any acceptance claim. It changes no guest input and does not
make a real Win16-dialog or OLE consumer pass claim.

## Selected graph and method

The formal x86 `wow32.dll` graph selects original
`src/mvdm/wow32/wcommdlg.c`, `wole2.c`, `walias.c`, `wkernel.c` and the
original COMMDLG dispatch table `wcmdgtbl.h`. It links the existing thin
`wow32-dll` callback and native-leaf bindings. The audit traced every
selected entry, its original callback/error/retirement path, and every
project-owned symbol with a common-dialog or OLE name. It did not treat the
immutable Win16 `COMMDLG`, `OLECLI` or `OLESVR` guest binaries as host source
or modify them.

## Common-dialog family

| Entry family | Original authority | Necessary host binding and its limit | Normal, failure and retirement authority | Disposition |
| --- | --- | --- | --- | --- |
| `ChooseColor`, `ChooseFont`, Open/Save and `PrintDlg` | `wcommdlg.c` owns every Win16/Win32 structure conversion, template lookup, hook setup, result copy-back, cancellation result and `CommDlgExtendedError` observation. `wcmdgtbl.h` is the original dispatch owner. | `wow_common_dialog_calls.c` is a seven-leaf scope guard around the native COMMDLG call. It owns no converted field, result, template, callback or error: it preserves the native result and `LastError` while ending the existing public USER scope. | Original bodies publish all guest outputs; native COMMDLG owns native HWND/HDC/global handles. Original template cleanup executes on every normal/error branch. | Keep the source-shaped binding; it is not a second dialog provider. S2 must exercise the selected original calls through real guest consumers. |
| Modal native hook callbacks | Original `WCD32CommonDialogProc` performs message thunking and `CallBack16`; `WCD32DialogProc` and `WCD32PrintSetupDialogProc` are its original registration names. | `wow_common_dialog_callback.c` enters only existing task/borrow scopes, refreshes the host window projection and then calls the renamed original bodies. | The original callback owns return conversion and guest entry; the scope wrapper restores task/borrow state on normal, nested and exceptional exits. | Existing `MVDM-HOST-DIV-335` is necessary, bounded and already registered. Do not duplicate the conversion algorithm. |
| Modeless `FindText` / `ReplaceText` | Original `WCD32FindReplaceText` returns the native HWND and registers `WCD32FindReplaceDialogProc`; the original `WM_DESTROY` branch owns COMMDLGTD/resource cleanup without invoking an application hook. | The same callback wrapper binds an original resource owner at `WM_INITDIALOG` and consumes it before original `WM_DESTROY` cleanup. It deliberately does not publish a synthetic whole-desktop result. | Original resource records and guest `FINDREPLACE16` storage remain authoritative; the wrapper only preserves entry/exit ordering. | Keep the one callback seam. S2 must cover modeless delivery and terminal task loss separately from modal paths. |

The actual thin-leaf fixture passed all seven leaves across normal, cancel,
reentry, rejection, native exception and finalization failure cases:
`COMMON_DIALOG_BOUNDARIES errors=0 leaves=7 scenarios=42 guest_proof=0`.
Its native standard-template reference also passed two modes. The actual
callback fixture passed original resource ordering, resource-only destroy,
early terminal cleanup and nested/exception/rejected-task guards. These are
boundary proofs, not real guest dialog acceptance.

## Related OLE family

| Entry family | Original authority | Modern-host boundary and ownership | Normal, failure and retirement | Disposition |
| --- | --- | --- | --- | --- |
| OLE2 foreign `THREADID` ↔ Win16 `HTASK` compatibility | Original `wole2.c::{Add,Find,Get,Remove}HtaskAlias` owns the selector-shaped 32-slot alias table, process/thread identity and creation-time validation. `walias.c` selects it for foreign thread IDs; `wuwind.c` consumes it for window ownership; `wkernel.c` requests the bounded image name. | The host supplies ordinary thread/process handles and `NtQuerySystemInformation`; entries store only scalar process/thread IDs and creation time. No host pointer/handle is passed to guest state and no project OLE registry exists. | Original full-table oldest-slot policy remains a stated compatibility limit. Explicit removal retires an alias; a stale identity is rejected by PID/creation-time comparison. The registered T452 bounded process-list/output divergence is the only selected host adjustment. | Reuse the existing selected original route. S3 owns real selected OLE consumer evidence, not a new COM/OLE provider. |
| Win16 `OLECLI` / `OLESVR` guest libraries | Immutable guest binaries and their original guest-side DDE/OLE behavior remain guest-owned. No selected WOW32 host source in this packet implements an OLE client/server. | There is no acceptable public, finite modern COM replacement in scope and no project substitute found. | This audit neither maps nor emulates guest OLE server lifetime. | Explicit stopping boundary; S3 may prove only a reached original OLE interface consumer. It must not import a broad OLE shell. |

The selected production `wole2.c` compile command was re-used for the alias
witness. It passed exact selector reuse, 32-slot oldest replacement, bounded
process-name output and explicit retirement:
`WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK`.

## Autonomous-implementation census

No duplicate COMMDLG conversion, template manager, callback registry, OLE
alias table, COM object model or OLE transport exists in `wow32-dll`. The two
dialog files are minimal host-entry/exit bindings required because original
code calls the host COMMDLG API and later re-enters guest code. They retain
one authority for each original state and are therefore kept. S1 found no
deleteable product implementation and no source-proven product defect.

Two test-only portability corrections were made: three observer scripts now
omit `Start-Process -ArgumentList` when a child has no arguments, and the OLE
alias observer passes its explicit file list to `Get-FileHash`. These repair
the current PowerShell invocation contract before any provider code runs; they
do not alter product selection or behavior.

## Finite successor boundary

S2 owns only common-dialog acceptance: real immutable guest consumers for
modal and modeless calls, cancel/error, hook reentry and cleanup, plus the
already-selected native scope/callback seams. S3 owns the independently
bounded OLE alias/consumer evidence. Neither successor may write a dialog
provider, a new resource registry, an OLE/DDE transport, or guest media.

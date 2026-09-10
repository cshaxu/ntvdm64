# M0 T396 S10 — visible WOW error-dialog implementation 001

## Question

Does the selected recovery preserve the distinct original error-dialog
families while replacing only the unreachable private CSRSS presentation
transport?

## Implemented boundary

`ErrorDialogBoxThread` in the original host source now takes its pre-existing
`ERRORPANEL` resource and `ErrorDialogEvents` procedure when `VDMForWOW` is
set.  Its existing `RMB_*` result processing and the original WOW
hung-task notification remain downstream of that presentation decision.

`WOWSysErrorBox` continues to call the adapter-owned `NtRaiseHardError`
entrypoint.  The entrypoint recognizes only the source-proven
`STATUS_VDM_HARD_ERROR | 0x10000000`, four-parameter, Unicode-mask `0xC`,
zero-response-options form.  It copies the two stack-owned `UNICODE_STRING`
values before creating a modal public-Win32 dialog.  It decodes only the
three original `SEB_*` descriptor slots, selects the declared default, and
returns the selected *source slot* (`1`, `2`, or `3`).  Unsupported forms and
calls without a session retain the prior controlled unavailable boundary.

No pointer or request is sent through a broker.  Direct original
`MessageBox`/`DialogBox` paths remain unchanged.

## Focused proof

`tests/adapter-mvdm-host-out/win32/t396_s10_wow_hard_error_dialog_fixture.c`
creates the exact supported request and finds the real modal dialog in a
worker.  It proves these cases without a fabricated result:

| Source descriptors | Selected control | Expected returned slot | Result |
| --- | --- | --- | --- |
| Close default in slot 1; Retry; Ignore | Close | 1 | pass |
| slot 1 absent; Retry default in slot 2; Ignore slot 3 | Ignore | 3 | pass |
| Yes default; No; Close | Close | 3 | pass |
| wrong hard-error status | none | unsupported / 0 | pass |
| unsupported descriptor 9 | none | invalid parameter / 0 | pass |

The fixture also verifies `DM_GETDEFID` before it sends its selected source
control, proving the declared default is installed in the dialog manager.
The current x86 run completed with `T396-S10 focused visible-dialog fixture:
PASS`.

## Build evidence

The current formal T310 graph compiled the affected units on both x86 and
x64 using its emitted compiler command, rather than a separate hand-written
compile profile:

- `obj/adapter-win32/wow_hard_error_dialog.obj`
- `obj/adapter-win32/wow_private_unavailable.obj`
- `obj/host/nt_error.obj`
- `obj/host/softpc-resource.res`

The x86 focused fixture links `user32.lib` and exercises the real dialog.
The x64 result is compile evidence only: the active product frontier remains
the x86 WOW process and must be observed through the complete-archive product
link before claiming a guest advance.

## Interpretation and next frontier

This closes the private-display dependency for the reached source families;
it does not emulate CSRSS or broaden `NtRaiseHardError`.  The remaining S10
exit observation is a complete current-binding x86 product run through the
former `wow:private-hard-error` stop.  A visible dialog is intentionally a
blocking, user-facing part of that observation, so the observer must record
the next owner after a source-defined selection rather than treat the modal
state as a timeout.

## Complete-archive runtime comparison

The S10 product was linked from the S8 complete-archive input set.  It retains
the four already-proved S8 replacement archives and replaces only the two
affected archives, with these exact members:

| Replacement archive | Replaced / added members |
| --- | --- |
| `original-softpc-host-roots.lib` | current `nt_error.obj` |
| `softpc-win32-bindings.lib` | current `wow_private_unavailable.obj`; new `wow_hard_error_dialog.obj` |

The product hash is
`c8117fe040ecb900d0b3fe0d6e83dbf89da19a816b6374cf15a14ec0bbeef9ce`.
The original-stage companion `WOW32.DLL` was restored as a manifest-verified
runtime companion before both comparison runs.

Under the historical S7 Console-owning observer and its exact
`system32\\krnl386.exe` command, the S8 product remains a valid control: it
exited after 4906 ms and emitted
`MVDM-SESSION-TERMINATION origin=wow:private-hard-error code=0x00000078`.
The S10 product instead remained live for the full 30000 ms, rose from the
control's 15 modules to 29 modules, and emitted no session-termination report.
It had not yet loaded `WOW32.DLL` at the observer boundary.

This is evidence that the current complete-archive S10 product no longer
takes the prior controlled termination.  It is not evidence that WOW32 client
loading has completed, nor does it identify the live post-boundary owner.
The next task must observe or source-map that live owner without replacing the
modal source-defined interaction with an inferred response.

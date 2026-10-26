# M0 T468 S4 — printing and spool acceptance

## Question

Can the selected original printing surfaces complete as one coherent package
without turning unavailable legacy-provider behavior into a new host policy?

## Inputs and procedure

The input package was the unchanged, hash-matched T467 package at
`build/t467-s3/package-r2`; its `WOW32.DLL` hash is
`506586EFB2B4C953C7D10468A592570E470209A6224A3B3D244880B03F7FFC23`.
Each checked-in runner used its historic modern PowerShell/private-desktop
path and a fresh, owned physical runtime stage.  The first attempted default
dialog run used an adjacent observer and emitted no target report; it is a
runner configuration failure and is not counted below.  The reconstructed
`console-startup-observer.exe` route was then used for every accepted case.

| Case | Entry point | Owned result | Assertion |
| --- | --- | --- | --- |
| HDC document | `verify-win16-print-probe.ps1` | `build/t468-s4/print-hdc-r1` | Pass: original StartDoc/AbortProc normal and cancel route, callback task return, two DC deletes, ProcInstance free, 1,490-byte PDF and no cancelled output. |
| Raw spool boundary | `verify-win16-raw-spool-profile.ps1` | `build/t468-s4/raw-spool-profile-r1` | Pass: original `OpenJob` returns zero without a legacy `[devices]` mapping; no host profile is changed. |
| PrintDlg default | `verify-win16-printdlg-default.ps1` | `build/t468-s4/printdlg-default-r2` | Pass: default conversion returns both 16-bit handles and original `GlobalFree` retires each. |
| PrintDlg cancellation | `verify-win16-printdlg-cancel.ps1` | `build/t468-s4/printdlg-cancel-r1` | Pass: original hook enters, ends the dialog through its documented path, and original `FreeProcInstance` succeeds. |

## Interpretation

The selected normal and cancellation routes have one original owner each:
the existing HDC/DC and callback route, and the common-dialog conversion and
application-owned returned global handles.  The raw route's missing legacy
profile mapping is an explicit original failure result, not a permission to
choose a modern provider or add a job manager.  The source audit in the
[S3 disposition](m0-t468-s3-printing-retirement.md) finds no missing original
task-loss cleanup binding that can be repaired without creating such a second
owner.

No production or guest source, guest media, host profile, provider policy, or
release artifact changed.  Accordingly this is acceptance evidence for the
unchanged published package, not a claim of a new release.

## Limits and handoff

The real legacy raw-job success path remains unavailable on this host because
there is no `[devices]` profile mapping.  The original device-capabilities
provider call also remains an explicit unavailable endpoint from S2.  Abrupt
raw-job task loss is retained as an original limitation, not a project-owned
resource-manager defect.  These facts close T468's selected capability scope;
they do not claim universal printer/provider compatibility.

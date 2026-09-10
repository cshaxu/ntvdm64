# M0 T276 S25: `fax` final integration disposition

## Decision

The original four `wowfaxui` user-mode bodies are final **adapter-backed**
providers. Their original UI, printer-installation and FAXDEV ordering remains
in the MVDM mirror; `adapter-wow`, `adapter-win32` and `session` replace only
the unavailable private WOW/shared-section transport.

`wowfax.c` is an NT4 private CSRSS/GDI-driver body and remains
**not-host-runtime**. It must never be silently replaced by a user-mode print
helper. Declarations are **binding-only**; all remaining product resources and
build inputs are exact evidence outside the final host link graph.

## Mapping rule

The historical shared section's 32-bit pointer/offset assumptions become
fixed-width copied broker records owned by `session`. The host-resource
mapping-manager instance may carry opaque host resources; no `PVOID` is cast
to a MVDM `DWORD`.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25FaxFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25FaxFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

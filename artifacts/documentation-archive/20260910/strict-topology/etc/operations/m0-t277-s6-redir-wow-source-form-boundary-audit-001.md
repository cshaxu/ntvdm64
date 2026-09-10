# M0 T277 S6 — Redirector and WOW source-form boundary audit

S6 completes the last 17 rows of the 325-row T277/S2 future-host worklist.
All remain implementation-disabled.

- Three Redirector CRT rows retain the normal original `getenv` form.
- `vdmredir/vrdisp.c:171` is the exact original segment:offset conversion
  body. Its `LPVOID` result cannot become a durable host pointer: the sole
  permitted successor is an `adapter-redir -> adapter-softpc` scoped session
  `guest_memory` lease or copied buffer.
- Original Redirector `DbgOut` remains diagnostic-only through
  `adapter-redir -> adapter-debugger`.
- Four WOW `host_simulate` calls preserve their saved-IP ordering but reuse
  the existing S3 Bochs mechanical route. They do not restore CCPU execution.
- Three debugger notification rows use copied fixed-width diagnostic records;
  no remote NT4 debugger transport is recovered by this task.
- Two MCI debug helpers remain `adapter-wow -> adapter-debugger` diagnostics.
- `W32Dispatch` retains the original WOW16-to-Win32 thunk entry shape through
  `adapter-wow`; guest state is accessed only via scoped mapping leases.
- The two WOW COM query entries retain their original BYTE/HANDLE forms, but
  native endpoint identity is represented through the session `host_resource`
  mapping instance before crossing a legacy-compatible ABI.

S3 through S6 now account for all 325 caller correlations selected by S2.
This is source-form closure, not proof that any one source body builds, links
or executes. The next T277 work is to reconcile these family results into the
global interface, package-edge and x86/x64 build-profile ledgers.

## Reproducibility

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T277S6RedirWowDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T277S6RedirWowDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

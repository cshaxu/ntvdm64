# M0 T276 S25 Redirector final expected-integration dispositions

## Conclusion

The original `vdmredir` DLL remains one owner package.  Its fifteen provider
bodies are `adapter-backed`: preserve dispatch, named-pipe/network/DLC
protocol order, asynchronous completion and failure semantics in the mirror,
then bind only external boundaries through `adapter-redir`, `adapter-win32`
and session-owned resources.

Six declarations are `binding-only`.  The four build/export/resource forms
are `not-host-runtime` evidence; they do not turn the provider into a generic
file-redirector shim.

DOS far buffers are copied through checked `guest_memory` leases.  Native host
handles and async IDs are session-owned and never appear in guest structures
or raw 32-bit fields.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25VdmredirFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25VdmredirFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

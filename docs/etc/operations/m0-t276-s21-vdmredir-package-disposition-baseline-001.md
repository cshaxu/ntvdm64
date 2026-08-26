# M0 T276 S21: `vdmredir` package disposition baseline

## Result

`vdmredir` is an original DLL owner package with fifteen selected provider
bodies. It combines Redirector BOP dispatch with named pipes, NetAPI/NetBIOS,
DLC/LLC, DOS far-pointer buffers and asynchronous completion. It cannot be
replaced by a standalone file-redirector shim.

Any later recovery preserves the original body and protocol ordering. Only its
external boundaries are adapted: session-owned host handles and completion,
`adapter-win32` public APIs, and checked mapping-manager copies for DOS memory.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Audit-T276S21VdmredirDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S21VdmredirDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

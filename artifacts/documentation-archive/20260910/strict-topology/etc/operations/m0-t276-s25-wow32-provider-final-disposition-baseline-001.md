# M0 T276 S25: `wow32` active provider final integration disposition

## Decision

The 77 active original WOW32 provider bodies remain one source-owned package,
but have final per-subdomain treatment:

- **overlay-required**: thunk/frame/structure; message/window procedure;
  GDI/window object; kernel/task/DOS session; and USER. These carry private
  NT4 USER/GDI/monitor contracts or unsafe 32-bit identity transport. The
  overlay preserves original provider control flow and replaces only the
  boundary transport with adapter and mapping-manager calls.
- **adapter-backed**: shell/OLE/printing, multimedia and network. Their
  original provider flow remains intact behind `adapter-wow` plus public or
  same-shaped `adapter-win32` capability bindings.

No `wow32` body is enabled by this audit. The decision prohibits pointer/HANDLE
casts in both 32-bit and 64-bit host builds and does not create a duplicate
mapping manager.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25Wow32ProviderFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25Wow32ProviderFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

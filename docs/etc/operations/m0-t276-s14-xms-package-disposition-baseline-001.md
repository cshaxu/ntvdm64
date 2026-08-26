# M0 T276 S14: `xms.486` package disposition baseline

## Source result

The selected original `xms.486` root contains fifteen paths: six literal
top-level XMS sources, one x86 architecture source, one non-x86 memory source,
one public header and six original build descriptions. The top-level historical
`sources` file builds the six provider sources; `i386/sources` separately
selects `i386/xmsmem86.c`. `xmsmemr.c` is not selected by the x86 profile.

All paths remain original-host mirror evidence and none is enabled.

## Essential boundaries

- XMS service dispatch stays in `xmsdisp.c`; `adapter-bop` carries only its
  mechanical ingress and completion ABI.
- A20, extended-memory range, block move, UMB and INT 15 effects require the
  existing `adapter-softpc -> adapter-bochs` machine boundary. Bochs owns the
  underlying memory, A20 and interrupt mechanics.
- `xmsumb.c` must not be treated as a simple handler. It needs a durable,
  bounded guest-byte UMB seam. Until that seam is proved, the source remains
  machine-profile-gated and preserves its original failure disposition.
- `i386/xmsmem86.c` casts 32-bit values to host addresses and calls NT virtual
  memory/RtlMoveMemory. Its three exported contracts are not discarded:
  `adapter-softpc` supplies the same names, parameters and result semantics,
  while the session guest-memory mapping manager resolves the 32-bit values to
  checked Bochs guest ranges. This applies on both x86 and x64; no host pointer
  passes through a BOP or guest register.
- `suballoc` remains an independently audited original support prerequisite;
  XMS does not receive a project-authored allocator.

## Later package order

The only valid future recovery order is: support allocator and session
contracts; same-shaped SoftPC/Bochs memory seam; complete XMS source package;
then monitor/DPMI consumers. Neither a `52:xx` trace nor one allocator function
can bypass the UMB, A20, IVT and bounded-memory prerequisites.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S14XmsDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S14XmsInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S14XmsDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S14XmsInterfaces.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S14XmsCompletion.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: 15 original paths and eight named, non-enabled interface
families, including an explicit mapping-manager replacement and UMB machine
gate.

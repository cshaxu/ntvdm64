# M0 T276 S15: `sim16` package disposition baseline

## Result

The eight-path `sim16` root is an original 16-bit **host-side WOW simulator
DLL** package. Its makefile uses `cl16`, `masm`, `link16` and original WOW16
libraries. It is therefore neither a Bochs-loadable DOS/WOW16 guest image nor
a host runtime library that can be directly linked into the modern process.

`sim16.asm` and `siminit.asm` implement a serial packet protocol that moves
VDM-memory requests, manipulates serial/PIC ports and switches stacks. Their
meaning is owned by the original WOW product shape. Any future revival must be
an `adapter-wow` package decision, with `adapter-vdm-monitor` owning monitored
guest-memory transport and `adapter-bochs` owning only serial/PIC mechanics.

## Consequences

- Keep all eight paths in the source mirror and preserve the original 16-bit
  build description as evidence.
- Do not move the assembler bodies into `opennt-guest-wow16`: they are not
  loaded into the emulated guest machine.
- Do not translate the serial packet protocol into a generic VDM service or
  create a second serial/PIC implementation. Current profile remains gated.
- The only later recovery path is complete WOW-package admission after its
  source-proven monitor and session contracts close.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S15Sim16Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S15Sim16Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

Expected result: eight paths, with both assembler bodies explicitly gated by
the original WOW product profile.

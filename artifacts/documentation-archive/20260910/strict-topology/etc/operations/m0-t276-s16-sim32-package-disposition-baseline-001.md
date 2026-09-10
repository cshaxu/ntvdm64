# M0 T276 S16: `sim32` package disposition baseline

## Result

The five-path `sim32` root is not the general SIM32 memory facade later used
by host control code. Its original `sources` deliberately names no translation
unit; `sim32.c` is a historical WOW serial-packet product shell talking to
`\Device\Serial1` and the 16-bit `sim16` peer.

The source's public `Sim32GetVDMMemory`, `Sim32SetVDMMemory`,
`Sim32GetVDMPSZPointer`, `Sim32FreeVDMPointer` and `Sim32SendSim16` API forms
remain useful evidence. When a source-proven caller is admitted, the modern
same-shaped facade must use the one session guest-memory mapping manager and
bounded checked bounce leases. It must not recreate the serial transport,
private device IOCTL path, a second mapping manager or raw guest pointer.

## Ownership

- `adapter-vdm-monitor` owns the recovered SIM32 API form.
- The session owns the unique guest-memory mapping-manager instance and lease
  lifetime.
- `adapter-softpc -> adapter-bochs` performs checked guest-RAM mechanics.
- `adapter-wow` owns the separately gated historical peer/product protocol.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Audit-T276S16Sim32Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S16Sim32Disposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

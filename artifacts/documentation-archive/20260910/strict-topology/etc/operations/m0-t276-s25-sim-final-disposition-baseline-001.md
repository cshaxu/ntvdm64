# M0 T276 S25 SIM16/SIM32 final expected-integration dispositions

## Conclusion

The historical SIM16 and SIM32 serial peer/product-shell bodies are
`not-host-runtime`.  They were host-side WOW products that coordinate through
a private serial device and stack/PIC protocol; they are neither Bochs guest
images nor a second machine implementation.

`sim32.h` remains `binding-only`: `adapter-vdm-monitor` must retain the
original `Sim32GetVDMMemory`, `Sim32SetVDMMemory`, `Sim32GetVDMPSZPointer`,
`Sim32FreeVDMPointer` and `Sim32SendSim16` call shapes for source-proven
consumers.  It resolves guest ranges through the single session
`guest_memory` mapping-manager instance and checked bounce leases.  It does
not recreate the private serial transport.

The SIM16 declaration forms remain `binding-only` source evidence for a later
complete `adapter-wow` package; its assembler bodies, macros, exports and
build inputs remain outside the final host graph.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25SimFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25SimFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

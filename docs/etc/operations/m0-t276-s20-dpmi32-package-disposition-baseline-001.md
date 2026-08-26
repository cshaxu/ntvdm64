# M0 T276 S20: `dpmi32` package disposition baseline

## Result

`dpmi32` is an original `LINKED_INTO_MONITOR` host library. Sixteen selected
x86 bodies form one DPMI provider: BOP dispatch, VDM_TIB context, LDT,
protected-mode exceptions/interrupts, DOS transitions and extended memory.
They cannot be split into trace-selected leaf implementations.

The original 32-bit code treats `Sim32GetVDMPointer` results as host pointers.
Recovery must retain the call ordering and data layouts but replace this
transport with the sole session guest-memory mapping manager: bounded lease or
copy, explicit direction/epoch and checked commit through
`adapter-softpc -> adapter-bochs`. Two RISC bodies remain source evidence only.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Audit-T276S20Dpmi32Disposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S20Dpmi32Disposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

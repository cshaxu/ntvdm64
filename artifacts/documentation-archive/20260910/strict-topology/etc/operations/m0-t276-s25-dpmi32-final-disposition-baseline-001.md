# M0 T276 S25: `dpmi32` final integration disposition

## Decision

The original `dpmi32` x86 provider is a final **adapter-backed** host package.
Its selector dispatch, protected-mode transition, LDT, interrupt, memory and
VDM_TIB algorithms remain in the original source bodies. `adapter-softpc`
provides the historical machine-facing interface shape, `adapter-bochs`
performs the mechanical request, and `adapter-vdm-monitor` binds only the
monitor callback boundary.

The original RISC-only memory bodies do not enter the supported x86/x64 host
graph. Private declarations remain **binding-only**, while original build
descriptions remain source evidence rather than runtime inputs.

## Address rule

The historical code's `Sim32GetVDMPointer` result is never a native host
pointer in this product. The sole session `guest_memory` mapping-manager
instance supplies a synchronous bounded lease or copy, with explicit direction
and checked completion. This preserves the original 32-bit field and call
shape without exposing Bochs RAM or an x64 pointer to MVDM code.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25Dpmi32FinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25Dpmi32FinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

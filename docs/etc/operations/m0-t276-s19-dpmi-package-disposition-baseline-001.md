# M0 T276 S19: `dpmi` package disposition baseline

## Result

The original `dpmi/makefile` builds `486/dosx.exe`, a 16-bit DOSX/DPMI guest
provider. It is not a host static library. Its selected object list comprises
22 guest provider bodies: protected/real-mode transition, INT 31h, LDT/memory,
interrupt reflection, BIOS, XMS/EMM/VCPI and WOW transition mechanisms.

All selected bodies remain one guest-package prerequisite. Future recovery
must preserve the whole guest product and its BOP/SoftPC boundary; it may not
link an individual `.asm` as a host provider or infer protected-machine
behavior from a trace. Host dependencies are later supplied only by the
source-shaped adapters and the unique session mapping manager.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Audit-T276S19DpmiDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S19DpmiDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

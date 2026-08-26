# M0 T276 S25 DPMI final expected-integration dispositions

## Conclusion

Every selected `dpmi/*` path is `guest-only`.  The original `dpmi/makefile`
builds `486/dosx.exe`, a 16-bit DOSX/DPMI guest product.  Its selected source,
alternate/gated source, macros, localization/build inputs and artifacts stay
with that one guest product and are never individually linked into the host.

The conclusion does not minimize the package's future importance: protected
mode, INT 31h, LDT, interrupt reflection, BIOS, XMS/EMM/VCPI and WOW contracts
still require the declared guest-to-host boundaries when DOSX is loaded.  It
only prevents the category error of treating a guest assembly unit as a modern
host provider.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25DpmiFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25DpmiFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

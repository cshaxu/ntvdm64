# M0 T276 S25 IEUVDDEx final expected-integration dispositions

## Conclusion

All 23 selected `ieuvddex/*` paths are `not-host-runtime`.  The original
product is the `ntvdm.dll` NTSD/WinDbg debugger extension, whose commands
inspect a different NTVDM process and render VDM state.  It is neither a VDD
provider nor an application runtime library.

Future debugger support has a single named boundary, `adapter-debugger`.  It
may retain extension-facing call/display contracts only through copied,
fixed-width session diagnostic snapshots.  It may not retain historical raw
process-address inspection, direct guest pointers or a private mapping table.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25IeuvddexFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25IeuvddexFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

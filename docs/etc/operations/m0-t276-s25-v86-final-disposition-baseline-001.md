# M0 T276 S25 V86 final expected-integration dispositions

## Conclusion

The seven historical kernel-VDM monitor bodies and the standalone historical
`ntvdm.exe` scaffold are `not-host-runtime`.  They depend on `NtVdmControl`,
CCPU/SAS and a duplicate startup path; importing them would recreate the
product shell that the modern `app`, `adapter-vdm-monitor`,
`adapter-softpc` and `adapter-bochs` boundaries intentionally replace.

`monitorp.h` and scaffold declaration files are `binding-only` source
evidence.  A later admitted monitor/app binding preserves only source-proven
call shapes, uses copied CPU frames and resolves guest ranges through the
single session `guest_memory` mapping manager.  `v86/util/*` is `tool-only`.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25V86FinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25V86FinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

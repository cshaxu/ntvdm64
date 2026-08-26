# M0 T276 S25: `softpc.new/host/src` device source disposition

## Decision

Ten historical video, FDC/DMA, timer/PIC and PPI/sound execution bodies are
**not-host-runtime**. Bochs is the sole final machine/device executor.

Eight host endpoint/control bodies are **adapter-backed**. They retain their
original host-side ordering for presentation, input, serial, printing, disk
file endpoints and resources. They bind through public host capability and
typed `adapter-bochs` operations only; no original body may access a Bochs
object or implement a device controller.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcDeviceSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcDeviceSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

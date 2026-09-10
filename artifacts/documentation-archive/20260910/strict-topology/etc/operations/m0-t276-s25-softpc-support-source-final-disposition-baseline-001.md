# M0 T276 S25: `softpc.new/base/support` source disposition

## Decision

`get_env.c` is an **adapter-backed** original configuration helper: its typed
lookup/default flow can bind to public host environment capability without
introducing machine state.

`ios.c`, `main.c`, `terminat.c`, `time_day.c` and `xt.c` are
**not-host-runtime**. They own an alternate IO table, CPU/register state, BIOS
timer or standalone SoftPC product lifecycle. Linking any of them would create
a second machine executor beside Bochs.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcSupportSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcSupportSourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

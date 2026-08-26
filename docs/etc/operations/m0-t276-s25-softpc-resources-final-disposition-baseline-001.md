# M0 T276 S25: `softpc.new` resource final integration disposition

## Decision

The 25 remaining selected `softpc.new` resource/build rows are not main
runtime inputs. The historical disassembler/debugger build inputs are
**tool-only**. Historical standalone SoftPC/NTVDM launch, link, icon and
resource inputs are **not-host-runtime**, because `app` owns final composition.

No source body is enabled or replaced by this decision. Every row remains an
exact mirror and source/provenance record.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcResourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcResourceFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

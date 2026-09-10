# M0 T276 S25: `softpc.new` declaration final integration disposition

## Decision

All 339 selected original `softpc.new` declaration carriers are final
**binding-only** inputs. They preserve historical names, layouts, constants
and calling conventions for source-parity MVDM bodies and their same-shaped
adapters. A header's presence never authorizes a second machine executor or a
direct dependency on a Bochs object.

## Boundary rule

`adapter-softpc` remains the MVDM-facing facade and reaches
`adapter-bochs` only for typed mechanical operations. Any pointer-shaped field
is governed by the session mapping-manager instances, not by the fact that an
original declaration happened to use a pointer type.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25SoftpcDeclarationFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25SoftpcDeclarationFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

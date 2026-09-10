# M0 T276 S25: `dos` final integration disposition

## Decision

All 27 selected original DEM and COMMAND source bodies are final
**adapter-backed** providers. The unit of recovery remains each complete
original provider package, never the next BOP observed in a trace. Original
dispatch order, data structures, algorithms and source-derived failure paths
remain in `opennt-mvdm-host`; only historical host/machine boundaries bind to
the named adapters.

The seven declaration carriers are **binding-only**. Four build descriptions,
directory metadata and the historical batch test are retained as exact source
evidence but are **not-host-runtime**.

## Boundaries

- `adapter-bop` carries only fixed-width BOP mechanics.
- `adapter-softpc -> adapter-bochs` supplies the historical SoftPC/SAS machine
  shape without exposing Bochs objects to the original providers.
- `adapter-win32`, `session`, `adapter-redir` and support packages supply only
  the reached host/session contracts.
- Pointer-shaped guest data resolves through a session mapping-manager
  instance; host identities never enter a DOS field.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Set-T276S25DosFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
powershell.exe -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-T276S25DosFinalDisposition.ps1 -RepositoryRoot O:\repos.hobby\ntvdm64
```

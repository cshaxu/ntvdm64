# T221 S3 P67 — Direct/Readonly handle-family matrix

## Question

Does the complete six-service `demhndl.c` handle family retain a coherent
Direct/Readonly profile disposition under the one `dem_whole_provider` rather
than depending on individual BOP fallbacks?

## Inputs

- OpenNT `src/opennt/base/mvdm/dos/dem/demhndl.c`
  (`demClose`, `demRead`, `demWrite`, `demChgFilePtr`, `demFileTimes` and
  `demCommit`);
- the T221 typed file-session/view/handle routes;
- `tests/bx-vdm/bx_ntvdm_dem_whole_provider_session_v1_test.c`.

The original translation unit is not independently composable because its
register, guest-address and handle macros bind it to the historical
SoftPC/CCPU/VDM host composition.  The provider is therefore a source-derived
rehost: it retains the `AX:BP` handle ABI, service ordering and success/failure
contracts while using checked adapter actions.

## Matrix

| Service | Direct | Readonly |
| --- | --- | --- |
| `50:00` seek | valid seek succeeds; released token returns `CF/AX=6` | valid seek succeeds; released token returns `CF/AX=6` |
| `50:02` close | closes typed direct token | closes typed readonly-namespace token |
| `50:08` time | get and self-owned-file set succeed | get succeeds; set returns `CF/AX=5` |
| `50:16` read | checked guest-RAM copy succeeds | checked guest-RAM copy succeeds |
| `50:1E` write | self-owned-file write succeeds | `CF/AX=5`, before host write |
| `50:27` commit | succeeds on the Direct token | source-shaped successful no-op |

The common fixture also keeps the opaque token boundary, generation invalidation
and no raw host-handle guest ABI checks active.  Existing Overlay and Virtual
paths are executed only as retained fixture coverage; this record adds no
behavior to either frozen profile.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\171-direct-readonly-handle-matrix `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The focused MSVC x64 `/MT` fixture compiled and returned exit `0`.

## Follow-up

This closes the Direct/Readonly regression matrix for the six-service handle
subfamily only.  The T221 whole-provider matrix and its remaining subfamily
closure audit continue; it neither closes DEM nor authorizes native trace work.

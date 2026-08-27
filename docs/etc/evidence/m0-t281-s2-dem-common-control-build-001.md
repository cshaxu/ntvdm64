# M0 T281 S2 — original DEM common-control build evidence

## Scope

This evidence records a selector-disabled static archive containing only the
original DEM common-control units `dem.c`, `demdata.c`, `demmsg.c`, and
`demdisp.c`. It is not a DEM provider, a BOP ingress route, or a runnable
image.

## Source and declaration decisions

- `dem.c`, `demmsg.c`, and `demdisp.c` are byte-exact OpenNT imports from
  `O:\repos.external\opennt\base\mvdm\dos\dem`.
- `demdata.c` retains only existing registered `MVDM-HOST-DIV-005`, `-006`,
  and `-007` numeric guest-location descriptor substitutions.
- The formal graph uses the existing `adapter-win32/include/nt.h` before the
  platform ABI include roots. This selects its same-shaped modern declaration
  carrier instead of recursively importing the historical private NT kernel
  header graph.
- `/DWIN_32` is specified by the original DEM `sources` file. `/DDEVL` is
  required by the original `demexp.h` declaration gate: it exposes the
  `fShowSVCMsg` debug mask and `demDebugBuffer` externs used by these original
  units. No debug service is run or linked.

| Unit | Current SHA-256 | Baseline disposition |
| --- | --- | --- |
| `dem.c` | `2573da2dbf964c5e75102145b031766113cfc4e914dc00f0d80fdc6d98f5a2de` | byte-exact |
| `demdata.c` | `d683f95923e4d5c9f31f5b1297910408b0eba68a1ac592d0cd9f54d364563c3c` | only `MVDM-HOST-DIV-005/006/007`; baseline is `366a350bc31579dc48764b81d28b26f4d2a5c69c66ec42d742ddaaf739e6a566` |
| `demmsg.c` | `1414099a0a5ad421440f101bbc6c669285141ab53bd9403b9ead4a6e4d9db2c8` | byte-exact |
| `demdisp.c` | `7ea3a829e3f8aad2dfe644d101cabaff514de25909db937675fc323dfce8fe3c` | byte-exact |

## Required formal commands

Run each command from an MSVC developer environment; build output stays under
ignored `build/M0-T281/S2`:

```powershell
powershell -ExecutionPolicy Bypass -File tools/build/New-T281OriginalDemCommonControlNinja.ps1 -Architecture x64
powershell -ExecutionPolicy Bypass -File tools/build/New-T281OriginalDemCommonControlNinja.ps1 -Architecture x86
ninja -C build/M0-T281/S2/x64
ninja -C build/M0-T281/S2/x86
powershell -ExecutionPolicy Bypass -File tools/governance/Test-T281OriginalDemCommonControlSurface.ps1 -Architecture x64
powershell -ExecutionPolicy Bypass -File tools/governance/Test-T281OriginalDemCommonControlSurface.ps1 -Architecture x86
```

## Required result

Both architectures produce only `original-dem-common-control.lib`. The graph
must contain no executable/run rule, BOP ingress source, DEM provider source,
or Bochs source. Any later provider composition is a separate admitted S.

## Observed result

On 2026-08-26, the required graph was generated and executed under external
MSVC Build Tools 2022 for both `x64` and `x86`. Each graph compiled all four
objects and produced `original-dem-common-control.lib`; its respective
selector-disabled surface verifier passed. The only diagnostic was MSVC
`C4996` at original `demdisp.c:233` for `sprintf`. It is an unmodified
historical debug-only call, not a compilation or source-composition failure.

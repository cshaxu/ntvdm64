# M0 T418 S4 worker ownership reorganization

## Scope and ownership result

The former worker-only `session` and `adapter-mvdm-host-out` implementation
families were relocated without changing their bodies or public protocol:

| Final owner | Relocated families |
| --- | --- |
| `src/ntvdm` | `session`, `command`, `softpc`, `monitor`, `redir`, `vdd`, `debugger`, `wow`, and worker-local `win32` implementations |
| `src/opennt-abi/host-compat` | historical declaration carriers and `opennt_support_rtl.c`, compiled separately into each consuming process |

The legacy `src/adapter-mvdm-host-out` root is absent.  No source was added to
either original mirror.  `host-compat` has no broker state, guest state or
cross-process runtime state; it is not a shared DLL or generic runtime.

## Fresh x86 verification

`tools/build/New-T310OriginalSoftpcNinja.ps1` generated
`build/M0-T418/S4/worker-closure-x86-002` with Node 22.  Its `build.ninja`
contains no selected `src/adapter-mvdm-host-out` path.  The sequential x86
build produced `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `VDMREDIR.dll`, and
the original SoftPC closure.

The following all returned zero:

- `ccpu-halt-reset-test.exe`
- `cvidc-vector-binding-fixture.exe`
- `original-external-memory-test.exe` (including EMS, SAS and DIB cases)
- `basesrv-reservation-test.exe`
- `basesrv-service-reservation-test.exe` (original `Check/Update/Get/ExitVDM`
  lifecycle through authenticated worker binding)
- documentation relative-link and governance verification; `git diff --check`

## Limitation

This is a no-behavior-change source reorganization.  Full deployed
COMMAND/MEM/EDIT/nested-COMMAND runtime acceptance remains T418 S5 work.

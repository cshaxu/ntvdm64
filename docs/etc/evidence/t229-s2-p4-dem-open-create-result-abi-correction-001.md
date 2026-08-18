# T229 S2 P4 — DEM open/create result ABI correction

## Finding

The whole-provider namespace route had reversed the original OpenNT file-size
register pair for `50:03 demCreate`, `50:12 demOpen`, and `50:22 demCreateNew`:
it wrote the low word to `DX` and the high word to `CX`.  For `demOpen` it also
used `BX` for the pipe flag.

`src/opennt/base/mvdm/dos/dem/demfile.c` defines the historical success
contract: the opaque file token is `AX:BP`; the 32-bit file size is `BX:CX`
(high:low); and `demOpen` alone returns its pipe indicator in `DX` (zero for
an ordinary file).

## Repair

`src/bx-vdm/bx_ntvdm_dem_namespace_partition_v1.c` now has one
`finish_open_result` seam used by the startup-readonly, Overlay, retained
Virtual, and Direct routes.  It returns:

| Result | Register contract |
| --- | --- |
| token | `AX:BP` |
| size | `BX:CX` high:low |
| ordinary `50:12` pipe flag | `DX=0` |

The seam changes no policy disposition.  Direct still invokes its admitted
host operation, Readonly retains its pre-host `AX=5, CF=1` rejection for
mutating creates, and Overlay/Virtual gain no newly admitted behavior.

## Verification

On 2026-08-18, formal Ninja rebuilt the affected `bx-vdm` static library and
fixture, then all current Direct/Readonly package regressions exited zero:

```powershell
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b bin/t225-s19-dem-package-lifecycle-fixture.exe
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly pdb-lifecycle
```

The fixture now asserts the complete `50:12` transaction in both modes: ingress
and provider selection, checked 260-byte guest pathname copy, checked two-byte
current-PDB owner read, Direct/Readonly open, exact success registers, then
the paired `50:02` close.  Its fixture-owned three-byte file proves `BX=0`,
`CX=3`, and `DX=0`; `AX:BP` is retained only as an opaque nonzero token.

## Scope limit

P4 corrects a shared source-derived result ABI across all retained provider
branches.  It does not close `demOpen`, `demCreate`, or the DEM package.
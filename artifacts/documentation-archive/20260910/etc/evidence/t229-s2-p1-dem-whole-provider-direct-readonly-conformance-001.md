# T229 S2 P1 — DEM whole-provider Direct/Readonly conformance

## Scope

This is a bounded implementation/conformance checkpoint for the active DEM
owner package. It proves selected source-derived paths through the installed
whole provider. It does **not** claim completion of the 29-service
file/handle/FCB/search group or the full 73-service DEM package.

## Source authority

- `src/opennt/base/mvdm/dos/dem/demfcb.c`
  - `demOpenFCB` / `demFCBCommon`: OEM `DS:SI` pathname and opaque
    `AX:BP` file identity; time/date and `DX:SI` size result layout.
  - `demGetFileInfo`: attributes/time/date and `BX:DI` size layout.
  - `demFCBIO`: registered DTA buffer; `BX` read/write selector; `AX:BX`
    size and `CX` transferred bytes.
  - `demCloseFCB`: `AX:SI` identity.
- `src/opennt/base/mvdm/dos/dem/demdir.c`
  - `demCreateDir` and `demDeleteDir` ordinary host namespace semantics.

The original bodies use CCPU/SAS pointer facilities and cannot be linked into
the CLI. The tested bx-vdm provider is therefore the smallest source-derived
seam: checked guest RAM through a mantle mechanical action, session-owned
opaque tokens, and ordinary Win32 host APIs.

## Exercised implementation

`tests/bx-vdm/bx_ntvdm_dem_package_family_v1_test.c`, explicit
`pdb-lifecycle` mode, now installs the real DEM whole provider in both product
profiles and proves:

| BOP | Direct | Readonly | Contract checked |
| --- | --- | --- | --- |
| `50:31` | host file metadata query | same read-only query | original `BX:DI` size shape |
| `50:2D` | OEM-path FCB open | read-only FCB open | opaque `AX:BP` token, no HANDLE escape |
| `50:2F` read | DTA write of file bytes | same DTA write | registered guest DTA, no SAS/host pointer |
| `50:2F` write | fixture-owned file becomes `XYZ` | `AX=5, CF=1`, file remains `FCB` | Direct/Readonly split at the owner boundary |
| `50:2E` | close using `AX:SI` | same | token is released |
| `50:04` | creates fixture-owned directory | `AX=5, CF=1`, nothing created | original namespace mutation disposition |
| `50:06` | removes that directory | not selected after refusal | Direct cleanup path |

All host mutation is confined to fixture-created temporary paths and cleanup.
No Bochs intrusion, guest-image mutation, Overlay behavior, or Virtual behavior
is introduced.

## Reproduction

```powershell
ninja -C build/M0-T229-S2/formal-direct-readonly-20260818b `
  bin/t225-s19-dem-package-lifecycle-fixture.exe
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe direct pdb-lifecycle
build/M0-T229-S2/formal-direct-readonly-20260818b/bin/t225-s19-dem-package-lifecycle-fixture.exe readonly pdb-lifecycle
```

All four commands exited `0` on 2026-08-18. `git diff --check` also passed.
The formal graph is generated from the pinned manifest; its build directory is
local and disposable.

## Remaining work

P1 does not close pathname create/delete/rename, file-handle open/read/write/
times/commit, FCB wildcard/volume-label cases, find-first/find-next, or the
Redirector-deferred pipe pair. T229 S2 remains active and continues by complete
owner-family coverage, not trace-selected leaves.
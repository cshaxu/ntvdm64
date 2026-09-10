# M0 T226 S2 — DEM package code-complete formal result

## Scope

This result closes the local code-complete gate for the OpenNT DEM `50:00..48`
owner package. It does not claim native guest execution, a guest trace result,
or completion of any non-DEM BOP owner package.

## Formal build

The pinned MSVC x64 `/MT` Ninja graph was generated in
`build/M0-T226-S2/fcb-direct-readonly-20260817a` with the projected Bochs
configuration hash `23B0A094B911C8663587BB62AE6A6E1AD4E109023F2D1EA6C7A555D032EE2877`.

## Local conformance result

All listed fixtures built and returned zero:

- whole `50:00..48` identity family in Direct and Readonly, plus Direct PDB lifecycle;
- error/lock, lifecycle, DASD/IOCTL, and source-proven no-op families;
- readonly file O/S/R/C, pathname search, Direct local-file backend, and opaque token lifecycle;
- drive/GSET/volume/DPB/CWD/DTA state;
- no-debug disposition, CurrentPDB copied identity, loader fast-read, and Direct host search;
- heap-backed FCB Direct/Readonly open/close/mutation-rejection and Direct `50:2F` mechanical I/O.

The fixture set uses checked copied guest-memory actions and temporary local
files only. Direct exercises permitted Win32 host capabilities; Readonly
proves the selected mutation failures. Overlay remains ABI-only and was not
selected as a T226 result.

## Code-complete disposition

The retained T225 source/ABI/failure maps and S1's 73 DEM bindings remain the
source inventory. This result upgrades their DEM package from route coverage
to local code-complete evidence: every identity is reached by the whole-family
fixture, while provider-specific and failure-sensitive families have focused
conformance fixtures. No typed route is presented as historical host-provider
completion by itself.

## Exclusions

No native trace was run. No Bochs semantic change, guest image edit, Overlay
implementation, COMMAND/XMS/DPMI/Redirector/Debugger recovery, or guest
multi-program execution claim is included.
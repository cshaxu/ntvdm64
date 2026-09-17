# M0 T420 S17 OEMUNI Package Recovery

## Selected package and owner result

The selected `src/mvdm/oemuni/sources` manifest contains exactly two
production sources: `file.c` and `process.c`. Their source selection is the
original OpenNT OEM/Unicode conversion package; there is no OEMUNI overlay,
adapter-owned duplicate conversion provider, or locally authored production
replacement in the formal product graph. `toemuni.c` is retained as the
original package test program rather than selected product code.

S17 found one source-native failure cohort, not a missing standalone service:
original cleanup can return an uninitialized local on early failure, and the
optional full-path file-part local has no failure sentinel. The only product
delta, `MVDM-HOST-DIV-269`, initializes `ReturnValue`, `FilePart`, `UString`
and `lpDstW` to their existing zero/null failure meanings. It leaves every
original conversion, allocation, Win32 call, successful result and cleanup
ordering intact. Thus the package adds no new provider and no policy to an
adapter.

## Focused proof

- The original `toemuni.c` suite compiled with the original `file.c` and
  `process.c`, existing formal OpenNT RTL/compat bindings, and a **test-only**
  no-pause stub. It completed OEM path conversion, current-directory,
  directory/file create-delete, temporary-path and temporary-file paths.
- A separate x86 failure fixture passed `GetShortPathNameOem(NULL, ...) == 0`
  with `ERROR_INVALID_PARAMETER`, and proved `GetFullPathNameOem` returns a
  non-null in-buffer file-part pointer for an OEM filename.
- A fresh formal x86 graph at `build/M0-T420/S17/formal-x86-001` completed all
  503 nodes and produced `run16.exe`, `basesrv.exe`, `ntvdm.exe`, `dtmgr.exe`
  and `VDMREDIR.dll`.
- All five artifacts were hash-verified after deployment to `O:\winnt`.
  Console-attached product regressions passed for `run16.exe MEM.EXE` and
  `run16.exe COMMAND.COM /C MEM.EXE`; both emitted normal MEM output and
  returned. An output-redirected, noninteractive harness can leave a worker
  waiting on its missing Console shape, so it is a harness limit, not counted
  as a product failure. The previously owner-accepted interactive COMMAND/MEM/
  EDIT route remains the established baseline; S17 does not alter Console,
  COMMAND, worker or broker behavior.

## Footprint

- Mirror production sources: two existing files, **15 added / 7 removed
  normalized source lines**, all `DIV-269` local-state initialization and
  rationale.
- Overlay production sources: **0 added, 0 retained OEMUNI-specific files**.
- Adapter/autonomous production sources: **0 added, 0 removed**.
- Test-only sources: one failure fixture, one pause stub and one Ninja
  generator; none participates in the product link graph.

S18 owns the next original package, `dpmi32`; its descriptor, protected-mode
stack, selector, interrupt and teardown lifecycle is not reopened by S17.

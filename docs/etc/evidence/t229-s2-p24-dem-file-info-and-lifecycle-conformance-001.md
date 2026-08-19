# T229 S2 P24 — DEM File-Info and PDB Lifecycle Conformance

## Question

Do the Direct and Readonly whole-package paths preserve OpenNT `50:01`
`demGetFileInfo` output ABI, and does `50:3C` keep its original PDB-termination
ownership without prematurely invalidating an SFT-backed file token?

## Original source contracts

- `src/opennt/base/mvdm/dos/dem/demfcb.c:532-590` defines `demGetFileInfo`:
  it opens the path read-only, derives file time/date/size, projects attributes,
  and returns `AX=attributes`, `CX=time`, `DX=date`, `BX:DI=size`, CF clear.
- `src/opennt/base/mvdm/dos/dem/demsrch.c:659-687` defines
  `demTerminatePDB`: it notifies the historical host and frees that PDB's
  FindFirst/FindNext list.  It does not itself close the DOS SFT handles;
  DOS abort later emits normal `SVC_DEMCLOSE` calls.

## Implemented result

- `bx_ntvdm_readonly_namespace_v1_query_startup_file` exposes only immutable
  profile-image size/time/date scalars.  `50:01` now returns the complete
  OpenNT register layout for COMMAND and other startup images; no host path or
  handle escapes the adapter.
- Direct/Readonly host files use the existing admitted root-handle namespace
  API, `GetFileTime`, local-to-DOS time conversion, `GetFileSizeEx`, and the
  existing attribute query.  This is read-only Win32 integration and does not
  change host state.
- The lifecycle fixture now follows the original ownership rule: `50:3C`
  clears its PDB search state; the outstanding Direct token remains valid until
  a normal `50:02` close.

Overlay remains an interface-supported later profile; this change neither
implements nor selects it.

## Reproduction

```powershell
ninja -C build/M0-T229-S2/formal-disk-reset-20260818r5 `
  bin/t229-s2-dem-whole-provider-session-fixture.exe
& build/M0-T229-S2/formal-disk-reset-20260818r5/bin/t229-s2-dem-whole-provider-session-fixture.exe
```

The fixture exited zero on 2026-08-18.  It exercises both selected profiles,
startup-image and admitted-host `50:01` paths, `50:42` fast-read continuity,
and the `50:3C → 50:00 → 50:02` lifecycle sequence.

## Boundary

This is a code-complete provider slice, not a claim that every DEM service or
native guest execution is closed.  It specifically strengthens `50:01` and
corrects the lifecycle test to original source semantics.
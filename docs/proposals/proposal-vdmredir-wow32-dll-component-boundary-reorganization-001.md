# Proposal — VDMREDIR/WOW32 DLL compilation-boundary reorganization

## Objective

After the active VDMREDIR runtime-package packet has passed its complete
DOS-guest acceptance matrix, make the two independently loaded historical
DLLs explicit product compilation components:

```text
src/ntvdm-exe/   -> ntvdm.exe worker-local product state
src/vdmredir-dll/ -> VDMREDIR.DLL target-owned bindings and build contract
src/wow32-dll/    -> WOW32.DLL target-owned bindings and build contract
```

This is a no-behavior-change reorganization.  It makes the loader, import,
export, TLS and worker-call boundary of each DLL inspectable without treating
a worker-local static library as shared DLL state.

## Invariants

- The canonical OpenNT MVDM mirror remains beneath `src/mvdm/` at its original
  relative paths.  In particular, `src/mvdm/vdmredir/` and the selected
  `src/mvdm/wow32/` paths are not duplicated, renamed or made subordinate to
  a project-owned directory.
- `src/vdmredir-dll/` and `src/wow32-dll/` are target components, not mirror
  roots.  They may contain only a target manifest, export/resource/build
  definitions and finite, registered DLL-to-worker bindings which cannot be
  original OpenNT code.  They do not contain a second session, emulator,
  BaseSrv policy, generic `common`, or compatibility layer.
- `ntvdm.exe` remains the sole owner of worker-local session/TLS, CCPU state,
  guest-memory leases, Console presentation and worker termination.  A DLL
  requesting guest access imports a narrow worker export; it must not link a
  second copy of a worker-local static library.
- `basesrv.exe` stays out of DLL-local state.  Its broker transport remains
  reachable only through the existing worker/client boundary.
- Existing `run16.exe`, `basesrv.exe`, `ntvdm.exe` and `dtmgr.exe` behavior,
  protocol/version checks and package layout do not change.

## Why this is needed

`VDMREDIR.DLL` is dynamically loaded into the worker, but its historical
source was being composed beside target bindings arranged under the worker
tree.  That is easy to misread as permission for DLL code to consume static
worker state.  The S20 NetAPI repair demonstrated the concrete failure mode:
a second static `session`/TLS instance cannot identify the guest executing the
BOP.  A separate DLL component makes its import boundary mechanically
auditable and prevents recurrence.  WOW32 has the same process-local loading
shape and deserves the same boundary before its package is recovered.

## S tasks

| S | Scope | Acceptance |
| --- | --- | --- |
| S1 | Freeze current target membership, original-source path provenance, exports/imports, static-library closure and TLS/session ownership for both DLLs. | A source-to-object-to-DLL ledger proves that no DLL links `session.lib`, a CCPU executor or worker-local state by accident; every existing adapter file has an owner/disposition. |
| S2 | Create `src/vdmredir-dll/`; move only VDMREDIR target-owned non-mirror definitions/bindings there, repair formal Ninja manifests and include paths, and preserve original MVDM inputs in `src/mvdm/vdmredir/`. | Fresh x86 formal link produces byte-compatible export naming and invokes the original `VrDllInitialize` through the declared stdcall entry adapter.  S20's complete guest matrix and established COMMAND/MEM/EDIT regressions pass unchanged. |
| S3 | Create `src/wow32-dll/` with the same target-only rule; classify every selected WOW32 binding before moving it. | Fresh x86 formal link proves one DLL-local closure and no second worker session/TLS; existing non-WOW product regression passes.  No WOW behavior is newly claimed. |
| S4 | Perform an independent boundary/diff review and remove obsolete worker-tree aliases, duplicate build inputs and dead target wrappers. | No live `ntvdm-exe/redir` or worker-local WOW target binding remains; source-policy and mirror checks pass; source/object/DLL ledgers, footprint report, governance and `git diff --check` pass. |

## Non-goals

- Completing, enabling, redesigning or excluding a VDMREDIR operation family;
  active T420 S20 owns that acceptance.
- Recovering WOW16 functionality, altering WOW32 callback semantics, adding a
  new broker protocol, or moving any original MVDM source out of the canonical
  mirror tree.
- Adding a generic shared library, direct DLL access to a session, or a second
  emulator/provider.

## Admission gate and verification

This candidate may be admitted only after T420 S20 has closed with its required
operation-family matrix.  Before and after each target move: run a fresh x86
formal build; inspect the DLL imports/exports and link maps; run all already
accepted direct and nested `COMMAND`, `MEM` and `EDIT` regressions; and run
the completed VDMREDIR guest matrix for the VDMREDIR target.  Record exact
added/deleted mirror, target-binding and adapter lines separately.

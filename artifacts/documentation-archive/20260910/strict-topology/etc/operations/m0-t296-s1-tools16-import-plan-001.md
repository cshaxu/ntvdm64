# M0 T296 S1 — OpenNT tools16 source import plan

## Objective

Complete the historical MVDM `tools16` build-tool input mirror at
`tools/opennt/tools16`.  This is a build-closure import only: the imported
executables and supporting files are never application, guest-image, host,
adapter, firmware, test, or default build inputs.

## Inputs and selection

- `O:/repos.external/OpenNT/base/mvdm/tools16`
- `O:/repos.external/OpenNT-4.5/nt/private/mvdm/tools16`

The implementation compares every relative path and SHA-256 in both trees.
Byte-identical dual files receive dual provenance.  A one-sided path or
content conflict receives one explicit selected source and records the other
candidate and reason in a manifest.  Existing matching files are retained;
missing selected files are copied byte-for-byte.

## Boundaries

- `tools/opennt/tools16` preserves the original `tools16` internal layout.
- It is not a `src/mvdm-*` component and does not participate in product
  compile, link, runtime discovery, or guest loading.
- No `src.old` path is read or used.
- No source file, component ownership, adapter, BOP or Bochs behavior changes.

## Exit criteria

1. Every selected source-tree `tools16` path has one manifest row and a local
   target with a matching SHA-256.
2. Dual-source equality/conflict and any selection are explicit.
3. The manifest records role and the non-runtime build-input exclusion.
4. Documentation governance and `git diff --check` pass.

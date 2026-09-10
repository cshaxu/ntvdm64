# M0 T284 S2 — component-root canonical rename closure

## Completed rename

T284 S2 applied the owner-approved names in one source-tree migration:

- `adapter-bop` → `adapter-mvdm-host-in`;
- `opennt-guest-dos` → `mvdm-guest-dos`;
- `opennt-guest-wow16` → `mvdm-guest-win16`;
- `opennt-mvdm-firmware` → `mvdm-softpc-firmware`;
- `opennt-mvdm-host` → `mvdm-host`;
- `opennt-mvdm-support` → `mvdm-support`;
- `opennt-mvdm-tools` → `mvdm-tools`;
- `opennt-platform-abi` → `mvdm-platform-abi`.

The dependent `mvdm-support-overlay` root and its matching test root were
renamed at the same time. The two inbound-bridge fixtures and the MVDM-host
and MVDM-support fixture roots were similarly aligned with their component
owners.

The owner explicitly authorized the four previously untracked Win16 mirror
directories (`drivers`, `system`, `timer`, `wfwnet`) to move with
`mvdm-guest-win16`; all eight files are staged as part of this closure.

## Reference and identity checks

- All old roots are absent from the live `src/` and `tests/` trees.
- A tracked-content scan finds zero old-root references in active source,
  tests, build/governance tools and current documentation. The intentional
  exclusions are `src.old`, `tests/legacy`, `tools/historical`, `docs/history`,
  `docs/etc/evidence`, `docs/etc/operations`, `docs/etc/legacy_code`, and
  `artifacts`: those are historical or generated evidence, not live inputs.
- The T284 S1 generator moved to `tools/historical/pre-t284/` because its
  purpose is to reproduce the pre-rename inventory; keeping it active would
  falsely retain obsolete product-root names.
- The existing source-relative content remains a Git rename except for
  required README/reference repairs. No OpenNT body, BOP route, provider or
  machine behavior was modified.

## Build verification

After the rename, the existing formal declaration-carrier graphs regenerated
and executed outside the sandbox with MSVC 2022 and Ninja:

```text
x86: build/M0-T283/S2/x86 — PASS
x64: build/M0-T283/S2/x64 — PASS
outputs: original-command-carrier.lib, original-dem-carrier.lib
```

Each architecture rebuilt the 11 selected original COMMAND and 16 selected
original DEM units from `src/mvdm-host`, through the canonical
`src/mvdm-platform-abi` declarations. Existing warnings from unchanged
historical source remain warnings; neither graph introduced a new error.

## Boundary

S2 establishes only canonical layout. It does not create
`adapter-mvdm-host-out`, migrate any of its source, alter ABI behavior, or
enable an MVDM product capability. Those changes begin in S3.

# M0 T266 S1 — `refs` live-consumer and disposition ledger

## Question

Which repository-tracked consumers still require the local `refs/` tree, and
what repository-local destination or explicit retirement path is required
before the tree can be removed?

## Inputs and procedure

- Repository state at T266/S1 admission.
- `git grep -l -I -e 'refs/' -e 'refs\' -- ':!docs/**' ':!.gitignore'` to
  enumerate every tracked non-document consumer.
- Recursive file counts for each existing `refs/` snapshot root.
- The formal Ninja manifest and generator, current component roots, CMake and
  tracked tool/test paths were inspected to distinguish formal/reached input
  from historical provenance.

The complete per-consumer machine-readable ledger is
[`m0-t266-s1-refs-consumer-disposition.tsv`](../operations/m0-t266-s1-refs-consumer-disposition.tsv).
It contains 49 tracked non-document consumer rows.

## Snapshot-root disposition

| `refs/` root | Files | Reached role | Required disposition | Packet |
| --- | ---: | --- | --- | --- |
| `archive` | 59 | mixed: default-disabled reconstructed-monitor/transport CMake inputs; other legacy/trace/probe source | tests/legacy for admitted fixtures; docs/etc/legacy_code or delete otherwise | S4 |
| `bochs` | 436 | formal config/include roots; current and historical Bochs tests/tools | src/bochs-core plus adapter-bochs; re-root current tests | S2 |
| `ms-dos-6` | 1395 | no tracked executable consumer | external provenance only; remove with refs | S5 |
| `ms-dos-official` | 1555 | no tracked executable consumer | external provenance only; remove with refs | S5 |
| `ntvdmx64` | 523 | historical comparison references only | external provenance only; remove with refs | S5 |
| `ntvdmx64-derived` | 14 | historical comparison references only | external provenance only; remove with refs | S5 |
| `opennt` | 47982 | guest staging plus BOP/DEM/provider inventory and provenance comments | selected paths into named src/opennt-* component roots; missing paths are controlled import rows | S3 |
| `opennt-45` | 4 | historical comparison references only | external provenance only; remove with refs | S5 |
## Observations

- The formal Ninja graph still reads `refs/bochs/config.h` and retains three
  `refs/bochs` include roots even though the manifest already identifies
  `src/bochs-core/config.h` as its logical source. That named file is currently
  absent, so S2 must perform an auditable Bochs mirror import rather than just
  changing a string.
- OpenNT governance/staging consumers still point at `refs/opennt`. S3 must
  map each reached original input to the matching `opennt-guest`, `opennt-bop`,
  `opennt-host`, `opennt-softpc`, or `opennt-utils` root. A path not present
  under an owner root is an explicit controlled import requirement, not a
  reason to retain the full snapshot.
- CMake's default-disabled reconstructed-monitor and transport fixtures read
  `refs/archive`. They are test/evidence material, not product source; S4
  must move retained code to a test/evidence owner or delete it.
- Documentation retains historical `refs/...` spelling as provenance. These
  non-executable records are not counted as live consumers and may remain
  after the directory is absent.

## Interpretation and follow-up

The source-closure task is feasible but not a simple delete: it requires S2,
S3 and S4 in that order, followed by a fresh repository-local build and a
scan that excludes only immutable historical documentation. No source was
moved and no runtime semantics changed in S1.
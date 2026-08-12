# Document Governance Follow-Up - 2026-08-07

## Scope

This follow-up audited the current authority documents against the completed
historical guest-toolchain rebase and the user-approved MS-DOS 6 comparison
artifact package.

## Findings And Resolution

1. `RESEARCH-PROJECT.md` and `ROADMAP.md` had an absolute ban on Microsoft
   guest binaries in the workspace/output area. That conflicted with the
   subsequently approved, hash-manifested comparison package. Both authorities
   now permit only segregated research artifacts below `artifacts/`; source,
   default build, runtime discovery, BYOB input handling, and release packages
   remain binary-free.
2. `docs/source-policy.md` did not define the retention contract for an
   approved comparison binary. It now requires a manifest, source/role/hash/
   byte-count/distribution record, and prohibits dependency or substitution.
3. `docs/planning/status.md` treated the completed guest rebase as active and
   described the 75-entry MVDM build-descriptor census ambiguously. It now
   records no active task, the four source-recovery blockers, and the fact that
   the 75 entries include host, tool, sample, and WOW targets rather than
   outstanding DOS guest payloads.
4. `docs/governance/document-map.md` retained an obsolete generated-artifact
   governance gap. It now points to the existing retention and source-policy
   authorities.

## Verified Current Scope

The direct NT4 DOS guest installation closure is 33 items: 22 source-built,
seven static data companions, and four fixed prebuilt reference inputs. The
four inputs have a separate source-recovery record at
`MSDOS6-GUEST-SOURCE-RECOVERY.md`. Five declared WOW16 device drivers are
separate, source-built, and byte-matched to their corresponding references.

The 75-entry `opennt-mvdm-artifact-inventory.json` is a source-tree build
descriptor census. It includes targets such as `ccpu386`, `dem`, `monitor`,
`ntvdm`, `sim32`, `video`, and `vdmredir`; it must not be used as a guest
payload completion count.

## Non-Changes

No historical-source or runtime conclusion was rewritten. No file was written
to `D:\home\repos.hobby\ntvdm64`.

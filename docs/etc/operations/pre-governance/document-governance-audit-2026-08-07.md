# Document Governance Audit - 2026-08-07

## Scope

This audit reviewed the repository documentation layout, root entry points, research and build evidence, local governance skills, and the read-only governance structure of `D:\home\repos.hobby\ntvdm64`.

## Findings

1. **No single documentation entry point.** Valuable material existed across the root and `docs/`, but there was no defined reading order or ownership map.
2. **Authority and evidence were mixed.** Roadmap, research reports, build records, and milestone notes could be mistaken for current policy.
3. **Historical-source constraints were implicit.** The primary OpenNT base, comparative source trees, minimal-change preference, and `ntvdm64` read-only boundary needed one explicit rule set.
4. **Current work state was not centralized.** Existing reports showed progress, but did not provide a one-task ledger with acceptance criteria and verification.
5. **Legacy records have provenance value.** Broad file movement or renaming would create churn and weaken links before a subsystem-by-subsystem review.

## Governance Applied

- Added `docs/README.md` as entry point and reading order.
- Added single-authority standards for documentation, sources, coding, and execution.
- Added architecture overview, document map, and current status ledger.
- Preserved existing evidence and root milestone records in place.

## Deliberate Non-Changes

- No historical source, build logic, or runtime behavior was changed.
- No existing research document was renamed, deleted, or rewritten.
- No file was written to `D:\home\repos.hobby\ntvdm64`.

## Follow-Up

Perform a focused cross-linking pass over the highest-value source, toolchain, artifact, ABI, and ROI records. Use the document map and source policy as the classification baseline.

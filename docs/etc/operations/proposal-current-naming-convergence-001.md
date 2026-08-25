# Current Naming Convergence

## Decision

All non-mirror repository material uses stable, semantic names. Historical project
labels, superseded component labels, and temporary generation suffixes are not
allowed in current source paths, symbols, build targets, tests, tools, current
documentation, schemas, or runtime-facing names. They remain verbatim only in
archived historical-record documentation.

An imported upstream mirror retains an upstream spelling only when that
spelling is part of the original external source identity or its observable
ABI; every such exception is registered in the mirror README. A temporary
local generation marker is never such an exception.

``legacy`` is reserved for archived historical records, immutable external spelling,
or historical-description prose; it cannot name a current ABI, facade, fixture,
or build input.

## Planned S Sequence

1. **S1 — complete naming inventory and semantic mapping.** Classify every
   live occurrence by owner, role, path, symbol surface, collision risk and
   historical-document exemption. Produce a deterministic old-to-semantic
   mapping before any broad edit.
2. **S2 — production component and formal-graph rename.** Rename all live
   component files, APIs, macros, schemas, targets and build-manifest entries;
   repair includes and links while preserving behavior.
3. **S3 — test, tool and current-document rename.** Move current tests/tools
   to semantic paths, repair invocation/configuration references, and remove
   non-historical documentation occurrences.
4. **S4 — full verification and historical boundary audit.** Prove the live
   negative scans, formal Ninja build/no-work result, fixture execution,
   documentation governance and clean working tree; record every permitted
   upstream or archived-record exception.

## Acceptance Boundary

This is a behavior-preserving source-layout and identifier refactor. It does
not change OpenNT, guest, Bochs, or host-service semantics. A collision or an
upstream ABI identity that prevents a safe semantic rename is documented before
an exception may remain.
# Original source and compatibility layout rename

Owner-approved active scope: rename the selected non-MVDM OpenNT source and
declaration cohorts beneath `src/non-mvdm`, and the project-owned shared
historical ABI bindings to `src/compat`. Keep `src/mvdm` as the selected
original MVDM mirror and preserve component-private adapters in their owners.

The original host and ABI cohorts contain three same-relative-path headers
with different contents: `ntexapi.h`, `ntrtl.h` and `nturtl.h`. Keep them
in `src/non-mvdm/host` and `src/non-mvdm/abi` respectively; retain their
selected include order and provenance instead of overwriting or merging them.
Update original code only where an existing host include names the old path;
record these minimal path-only divergences. Preserve guest binaries and all
unrelated edits.

Validate build graphs and all source paths, cached x86/x64 builds, compiled
image comparison and proportionate DOS/WOW/Win3.1 regressions. Publish and
hash-check the complete product package at both destinations, run required
governance, and commit/push. The original input-source handoff is deferred
to the subsequent S without losing its scope or evidence.

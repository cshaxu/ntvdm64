# Canonical OpenNT MVDM source-tree recomposition

## Objective

Recompose every selected original MVDM source/input currently split among
`mvdm-host`, `mvdm-guest`, `mvdm-tools` and `mvdm-softpc-firmware` into one
canonical `src/mvdm/` tree whose relative paths match the selected OpenNT
`base/mvdm` union. This makes direct visual/source-control comparison with
OpenNT meaningful without changing source contents or build ownership.

The logical build classifications remain explicit: executable host source,
load-only guest source, independently buildable tools and immutable firmware
inputs. A physical source root does not authorize a new link edge. Existing
non-MVDM `opennt-host`, project code and adapters remain outside this tree.

`src/mvdm-host-overlay` is not original OpenNT content. Re-root it as the
private paired `src/mvdm-overlay` tree, retaining its same relative source
paths and mirror-private restriction; it must not be mixed into `src/mvdm`,
where it would make byte comparison ambiguous. `opennt-host-overlay` is out of
scope.

## Sequential S plan

### S1 — Freeze canonical path map and selected graph

Create a complete, reproducible old-to-new path manifest for every tracked
selected MVDM file. Freeze source identities, SHA-256 equality requirements,
the allowed OpenNT relative-path mapping and the current formal production
generator/stager/manifest consumers. Classify each root as host, guest, tool,
firmware or overlay; identify any non-original project file that must remain
outside `src/mvdm`. Audit case-only and path-collision hazards before moving.

Exit: every selected tracked file has one destination or an explicit excluded
reason; all formal build/stage consumers are enumerated; no source move or
content change has occurred. This is the reference for S2 identity proof.

### S2 — Mechanical re-root and composition repair

Use `git mv` only for admitted source-path moves. Move selected original
material below `src/mvdm/<OpenNT-relative-path>` and the private MVDM overlay
below `src/mvdm-overlay/<same-relative-path>`. Update only current production
build generators, manifests, staging scripts and active architecture/source
layout authorities to consume the new paths. Keep logical package roles in
build selection and do not bulk-rewrite historical evidence or retired tools.

Exit: every S1 mapped file has the exact same blob/hash at its destination,
the old MVDM roots are absent, no new source root duplicates the original
tree, and selected build input remains host/guest/tool/firmware-correct.

### S3 — Build, visual-diff and governance closure

Generate a fresh x86 /MT formal graph and build the three-program package.
Run the selected COMMAND/MEM/EDIT and broker regressions plus staging manifest
verification; update `O:\winnt` only with tested formal executables and write
runtime logs below `O:\winnt\logs`. Run source identity/path comparison against
the pinned OpenNT union, verify overlay exclusion from the visual original
tree, documentation governance, links and final diff review.

Exit: all mapped original blobs are identical, all current formal consumers use
the canonical tree, accepted runtime regression passes, no untracked product
input remains, required records are committed/pushed and the worktree is clean.

## Boundaries

This is a layout/recomposition task, not a source-recovery, CCPU, DPMI, WOW or
broker behavior repair. It neither deletes overlays nor reclassifies a private
overlay as original source. It must preserve the source-policy stopping
boundaries, the one CCPU40/x86 product profile, existing guest load-only rule,
and current adapter/broker dependency directions. Any content difference,
unmapped collision, changed build ownership or behavioral regression pauses
the task for explicit owner review.

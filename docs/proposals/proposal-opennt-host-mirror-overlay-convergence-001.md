# Proposal — OpenNT host mirror and overlay convergence

## Objective

Bring `src/opennt-host/` under the project's mirror-component review standard.
Every production file must have a known OpenNT baseline and exactly one
disposition: exact original, true original subset, same-shaped
original/subset modification, private `opennt-host-overlay` implementation, or
transfer to its proper non-mirror owner.

## Scope

- Inventory every live `opennt-host` source, header, build input and host-route consumer.
- Compare each claimed mirror file with its selected OpenNT baseline.
- Register every retained subset or divergence locally with `DIVERGENCE:` and in `src/opennt-host/README.md`.
- Move a file whose changed executable/data-bearing content exceeds the mirror threshold into the corresponding `src/opennt-host-overlay/` path, leaving only the smallest registered call boundary in the mirror.
- Move project-authored compatibility code that is not an OpenNT host mirror to its actual owner, normally `adapter-win32`, `adapter-softpc`, `session`, or `app`.

## Non-goals

- No new host capability, BOP behavior, Win32 feature, or OpenNT package is admitted merely to improve mirror coverage.
- No dormant OpenNT package is copied into production as local reference material.
- No public overlay ABI: `opennt-host-overlay` remains private to `opennt-host`.

## Required evidence

The admitted task records a file-by-file baseline/disposition ledger, changed line counts for every modified mirror, README exception-register reconciliation, and focused route/build verification for every retained live host path. It must also demonstrate that no non-host component imports the private overlay.

## Admitted sequence

1. **S1 — baseline and disposition:** identify the selected OpenNT original,
   hash and line-difference disposition for every live host and private-overlay
   unit; record each direct source/include/link consumer. No source body moves.
2. **S2 — mirror/overlay boundary repair:** add any missing crop and modified
   expression markers, reconcile the host README register, and move only an
   oversized or unshaped binding body into the already-private overlay.
3. **S3 — host owner-edge repair:** remove any direct cross-component source
   inclusion or non-host implementation from the host component by an
   explicitly reviewed original-owner disposition. This does not add a new
   host capability.
4. **S4 — formal closure:** verify the repaired source ownership, README and
   overlay privacy, then run the focused host route and formal Ninja build.

Each later S is admitted only after S1's ledger establishes its exact file and
boundary scope. A source-body change which alters a reached host failure path
is a stop condition requiring renewed owner approval.

## Exit criterion

`src/opennt-host/` contains only audited OpenNT mirror material and its README fully indexes every non-exact unit. Every oversized or non-mirror body has been moved to its proper private overlay or component owner, with no behavior change claimed beyond the verified existing host routes.

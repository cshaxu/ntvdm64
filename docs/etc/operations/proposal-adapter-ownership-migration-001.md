# Complete adapter ownership migration

## Purpose

Make `adapter-mvdm-host-out` describe current ownership rather than historical
accumulation.  In particular, leave `softpc` with only CCPU/SoftPC machine
mechanics and move every non-machine implementation, declaration, build input
and test reference to its one final owner.  This is a complete architecture
migration, not a partial cleanup before another recovery attempt.

## Why it precedes DPMI32/WOW32 runtime work

The original DPMI32 and WOW32 owners both use host-facing seams.  The current
`softpc` family contains machine mappings beside COMMAND, VDD, package-media,
termination and observation semantics.  Admitting or diagnosing the original
providers while those meanings are mixed would make a missing provider look
like a machine dependency and entangle a source recovery with a directory
migration.

## Required end state

1. A complete tracked owner ledger covers every file now below
   `src/adapter-mvdm-host-out/softpc`, including public headers, production
   implementations, tests, build references and generated-graph inputs.
2. Each row has exactly one final semantic owner, original caller(s), ABI
   surface, dependency direction, migration operation, and verification row.
   `unknown`, `temporary`, dual implementations and an unowned compatibility
   copy are invalid dispositions.
3. `softpc` retains only original SoftPC/CCPU machine mechanics: synchronous
   guest-memory/physical mapping, A20, effective-address/descriptor handling,
   machine execution and machine-shaped host identity where its original
   caller requires it.
4. COMMAND/BaseVDM, VDD/DOS-WOW shared state, lifecycle/observation and
   immutable package-media selection receive distinct architecture-approved
   owners.  S1 must decide whether each belongs in an existing specialist
   family, `session`, `app`, or a newly admitted narrow specialist family; it
   may not assume a new generic helper root.
5. Every relocated public header has one compatibility forwarding path only
   for the bounded migration interval.  The forwarding file contains no
   implementation, and every caller/build reference is then moved to the
   final path before closure.
6. The default formal product and focused fixtures have identical source
   behavior before and after the migration.  The work does not change a BOP,
   guest byte, source-owned MVDM control flow, machine backend, media
   selection result, or runtime acceptance claim.

## Work sequence

### S1 — Full ownership inventory and final placement decision

Freeze the current source/build/test consumer graph.  Produce the complete
ledger and decide every final owner with architecture review.  Record each
unavailable private contract as a named boundary, not as a reason to leave
the file in `softpc`.

### S2 — Mechanical one-owner migration

Perform the approved moves and forwarding-only compatibility transition.
Update include paths, source lists, Ninja generators, fixtures and current
architecture documentation.  Preserve exported C signatures, status/failure
rules and session ownership exactly.

### S3 — Remove transition paths and prove graph equivalence

Remove every forwarding path after all consumers use the final owner path.
Verify that no non-machine implementation remains below `softpc`, that each
ledger row has one owner, and that the formal x86/x64 compile/link inputs and
focused non-runtime fixtures remain behaviorally equivalent.

## Exclusions

This package does not admit DPMI32 or WOW32 source changes, new BOP behavior,
WOW provider runtime success, guest-media work, CPU changes, external source,
or an unconstrained `compat`/`common` family.  DPMI32 and WOW32 recovery begins
only after this migration closes.

## Acceptance

Closure requires a zero-unclassified full ledger, no non-machine body under
`adapter-mvdm-host-out/softpc`, no dual implementation or lingering forwarding
file, a reviewed build-input comparison, focused ABI/negative tests, and
architecture documentation that names every final family and its dependency
direction.

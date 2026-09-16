# Mirror-private component elimination and project-component reorganization

## Objective

Eliminate all three temporary/private source-component roots:

1. `src/mvdm-softpc-patch`;
2. `src/mvdm-overlay`; and
3. `src/opennt-host-overlay`.

Their content must become either a minimal, registered source change in the
proper original mirror (`mvdm` or `opennt-host`) or a narrowly named
`adapter-*` binding where the original owner cannot call the modern platform
directly. It is not acceptable to preserve a directory by renaming it, or to
move original provider policy into an adapter simply to make the directory
disappear.

The final phase separately audits `app`, `broker`, `session`,
`adapter-mvdm-host-in`, `adapter-mvdm-host-out`, and `adapter-opennt-host` and
produces an owner-reviewed component-reorganization plan. It does not silently
implement that plan as part of this package.

## Governing placement test

Every moved line is classified before it moves:

| Content kind | Final location |
| --- | --- |
| Original OpenNT/SoftPC declaration, algorithm, or same-shaped small correction | Its original-relative path under `src/mvdm` or `src/opennt-host`, with a `DIVERGENCE:` register row where bytes differ. |
| Bounded modern OS/ABI binding required by a reached original call | The named `adapter-mvdm-host-*` or `adapter-opennt-host` family that owns that external interface. |
| Project lifecycle, cross-process coordination, or product policy | `session`, `broker`, or `app` only when it actually belongs to that component's declared responsibility. |
| Provenance-only patch excerpt or retired diagnostic | Indexed `docs/etc` evidence or deletion; never a production source component. |

The packet must first attempt direct original composition, then the smallest
same-shaped binding. A relocation counts as **zero net code reduction**. Any
proposed destination that changes ownership, ABI, ordering, or failure behavior
is a stop condition until separately admitted.

## Four phases

### Phase 1 — eliminate `mvdm-softpc-patch`

Freeze every current include, object, library, link and symbol edge. Then
move its content directly into the correct `mvdm` owner:

- the generated CCPU/SAS/GDP declaration carriers enter their original include
  owner path under `mvdm` after recovering a byte-identical original input when
  available;
- the existing `WINAPI` and exception-scope corrections remain minimal mirror
  diffs in their already-correct `mvdm` callers; and
- each `fmstubs.c` symbol is independently recovered, deleted if unreachable,
  or placed as the smallest source-shaped correction in its original `mvdm`
  owner. It must not remain a separate patch provider.

Patch excerpts that are only provenance move to indexed evidence. Closure for
this phase is zero tracked paths and zero formal include/object/library/link
edges under `src/mvdm-softpc-patch`.

### Phase 2 — eliminate `mvdm-overlay`

Audit every overlay file, hook, generated binding and formal graph edge. For
each, recover or merge the smallest body into its rightful destination:

- CCPU/C-VID declaration and vector setup belongs with its selected MVDM
  owner, unless it is exclusively a modern ABI binding, in which case it goes
  to the existing named SoftPC adapter family;
- original-wrapper subsets such as SAS invalidation and EOI conversions return
  to their MVDM owner when their type/ABI contract is directly expressible;
- DOS, DEM and monitor additions return to their MVDM callers or to the named
  external-interface adapter, never to a generic helper; and
- generated code is either recovered from the original generator/input,
  absorbed as a small owner-local generated artifact, or deleted with all of
  its graph edges.

Closure for this phase is zero tracked paths and no formal production reference
under `src/mvdm-overlay`, plus focused ABI and CCPU40 regressions for every
former provider.

### Phase 3 — eliminate `opennt-host-overlay`

Audit each private OpenNT-host overlay body and its original-package caller.
For `dos_worker_wait.inc`, first attempt a source-shaped minimal body inside
the original BaseSrv `srvvdm.c` mirror; if it is solely a modern transport
binding, move it to the BaseSrv subfamily of `adapter-opennt-host` while
keeping record lookup, locking and queue policy in `opennt-host`.

Closure for this phase is zero tracked paths and no formal production reference
under `src/opennt-host-overlay`, with resident-worker versus pending-command
and broker-disconnect regressions passing.

### Phase 4 — audit project-component relationships and propose reorganization

Produce a complete, non-mutating relationship audit for `app`, `broker`,
`session` and every `adapter-*` family. It must list each production public or
cross-component interface, caller, callee, process boundary, state owner,
thread/cleanup owner, original-source owner where applicable, and current
build/link direction.

The resulting plan identifies:

- duplicate policy or mechanics that should be consolidated;
- original semantics wrongly held in `app`, `broker`, `session`, or adapters;
- adapter families that are too broad or have crossed into provider policy;
- safe destination, ABI migration, test matrix, expected line/file movement,
  and compatibility risk for every proposed reorganization; and
- changes that require separate task admission rather than implementation here.

This phase ends with a proposal and no unreviewed source reorganization.

## Verification and closure

After each phase, regenerate the formal MSVC Win32/x86 `/MT` CCPU40 graph and
compile affected owners. At package closure run the focused provider fixtures,
COMMAND/MEM, nested COMMAND return, EDIT keyboard/mouse, EDIT-exit then MEM,
and broker worker-death/pending-command contracts. Preserve known WOW limits
honestly rather than using this cleanup to claim WRITE completion.

The final report separately quantifies:

- deleted `mvdm-softpc-patch`, `mvdm-overlay`, and `opennt-host-overlay` files
  and lines;
- lines merged into original mirrors, adapter bindings, and project components;
- genuine net deletion versus relocation; and
- residual mirror diffs with original owner and rationale.

No source is allowed to remain in any of the three eliminated directories, and
no build, manifest, architecture rule or production documentation may name
them as a live component.

# Task Identifier Governance

## Governance Delivery

Documentation governance uses one standalone S sequence without allocating a
numeric T or displacing the active product packet in Status:

| Delivery | Result |
| --- | --- |
| M0 Td S1 P1 | Initial local documentation-governance delivery. |
| M0 Td S1 P2 | MTSP realignment: single active S, T-only Queue, and retained status ledger. |
| M0 Td S1 P3 | Complete-document inventory and hash-verified migration control. |
| M0 Td S1 P4 | BOP owner-package sequencing governance. |
| M0 Td S1 P5 | Merge no/low-dependency and purely internal BOP closure into one no-external-dependency candidate T; reconcile T237 and tracker closure facts. |

## Hierarchy

```text
M<milestone> T<task> S<subtask> P<part>
```

- **M** is a large milestone outcome with a roadmap gate and closure audit.
- **T** is an owner-approved, medium-sized task package from `QUEUE.md`.
  Numeric T values are global, strictly ascending, and allocated only when
  admitted to the single active `STATUS.md` packet.
- **S** is the smallest bounded implementation, review, and acceptance goal
  within one active T. It has a complete status packet and cannot expand
  materially without a packet revision.
- **P** is one accepted, committed, and pushed delivery of an S. Corrections
  use the next P; an accepted P is not rewritten.

`M0 T95` closed by owner-authorized reprioritization on 2026-08-11; its
closure record is `../../history/m0-t95-closure-20260811.md`. T96 through
T225 are retained closed historical packages. The sole active implementation
package and its next delivery serial are read only from `STATUS.md`; historical
labels do not reserve or reopen another T.

## r Run Records

Lowercase `r<n>` labels (`r1` through `r23` currently) are **legacy local
run/evidence iterations**. They name a configuration, build, fixture, trace,
or evidence-export attempt; they are neither a T nor an S. They predate the
restored commit-backed P lifecycle and therefore must not be retroactively
declared accepted P deliveries.

| Current package | Run range | Classification | Formal P disposition |
| --- | --- | --- | --- |
| `M0 T95 S1` | `r1`--`r23` | Historical Bochs/adapter mechanics and first-profile evidence iterations. | Retained as evidence aliases only; no P is backfilled. |
| Later `M0 T95 S<n>` | New run(s) | Retained T95 evidence where present. | No P is backfilled during closure. |
| Active `M0 T<S>` packet | New run(s) | Reproducible build/fixture evidence for the admitted S. | The accepted commit is `P<n>`; it links its run IDs, rather than renaming them. |

A P may cite one or more r runs, including failed diagnostic runs, but P is
the accepted commit/review boundary. A run cannot allocate a T or S, close an
S, or substitute for a reviewed commit. This preserves the meaning of existing
`r20`, `r21`, `r22`, and `r23` records without inventing historical commits.

## Td Documentation Work

Standalone governance, roadmap, audit, or documentation work uses
`M<milestone> Td`, optionally with `S<n>` and `P<n>`. It does not allocate a
numeric T, artifact revision, or product-build requirement. Its durable record
is the current-status summary, commit, and relevant evidence record. This
repair is **M0 Td S1 P1** when accepted and committed.

## Legacy Records

`M0`--`M92`, `G0.4`, `R1`--`R5`, and prior `S1`--`S6` research labels are
retained historical evidence. They cannot be converted into current T numbers,
reopened as current tasks, or used to fill gaps in the T sequence. Cite them
as legacy aliases only.

## STATUS And QUEUE

`STATUS.md` holds exactly one active numeric T packet and exactly one active S
packet within it. The packet states the full `M T S` identifier, mode,
approval, objective, non-goals, baseline, affected surface, applicable rules,
verification, expected markers, assets, reporting requirements, stop
conditions, exit criteria, original request, and similar-issue sweep.

`QUEUE.md` owns the ordered list of **unnumbered candidate T packages**.
Queue priority is not approval or numeric allocation. It contains no active
packet, S/P entry, technical baseline, or T identifier. The active T and its
one active S live only in Status; any multi-S sequence is a supporting
operation plan under etc/. After the current T closes, admit the selected
queue candidate to Status and assign the next global T serial at that time.
A Td summary may appear in Status but never displaces the active numeric T
packet.

## Execution Modes And Gate

Every T declares either Ordinary Mode or Coordinated Dual-Session Mode. In
dual-session mode, the coordinator alone admits/replans/closes T and S work,
reviews actual changes, and forms P commits; the executor independently
reviews the brief, implements, gathers evidence, and reports material scope or
risk objections. One active S exists at a time in either mode.

Before accepting a P or closing a Td task: inspect actual worktree changes,
re-read the original request, verify every packet requirement, run the
documentation gate and `git diff --check`, record out-of-scope disposition,
then commit and push. A push failure blocks acceptance unless the owner grants
a documented bounded deferral.

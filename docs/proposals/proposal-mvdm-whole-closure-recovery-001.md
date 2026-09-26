# Proposal — MVDM whole-closure completeness audit and recovery

## Objective and admission

Owner-requested candidate, ordered before implementation minimization: audit
and repair original OpenNT MVDM
code missing from this repository, and original code already imported but
not actually used. Recover MVDM as a coherent whole closure, rather than a
collection of individually selected working fragments. Reduce mirror diffs
and autonomous implementation footprint by restoring the original owners.
This is an audit-and-repair package, not an inventory-only delivery.

This proposal does not allocate a numeric T, admit implementation, or change
the current active packet. Assign its T number through CURRENT.md when the
owner admits the candidate. Freeze the then-current baseline: earlier queue
work may already have repaired some gaps.

## Governing principle

Whole-package inclusion and original-owner reuse are the default. Excluding
an original part requires evidence of a serious integration problem or an
unavoidable constraint, not convenience, lack of a current trace hit,
unresolved symbols, or the fact that COMMAND and EDIT already work.

Distinguish source completeness from executable composition. Guest source,
immutable guest media, firmware, historical tools and architecture-specific
alternatives remain original parts of the inventory, but their legitimate
roles do not make them host link inputs. An unused host provider is not
automatically legitimate merely because its file exists in the mirror.

Original OpenNT files may be imported at their original relative paths with
provenance and dependency review. Project-invented files may not be disguised
as mirror files. Apply the current [source policy](../etc/operations/policy/source-policy.md),
[architecture rules](../rules/ARCHITECTURE.md) and
[coding rules](../rules/CODING.md). Preserve immutable guest media and the
selected x86 CCPU40 product. A demonstrated conflict with a hard product
boundary requires an explicit disposition or owner decision, not a silent
omission or an unauthorized revival of a retired backend.

## Complete audit scope

Start with the pinned canonical OpenNT/OpenNT-4.5 MVDM source union, not only
the current build manifest or a list of previously accepted packages. Retain
edition conflict decisions and source identities; do not silently combine
incompatible alternatives. Reuse existing provenance and package ledgers,
then independently reconcile them with actual files and build artifacts.

For every original package and path, establish:

- upstream identity and role; whether the file is absent, complete, cropped,
  or modified locally;
- declared build selection versus actual compilation, archive extraction,
  final link, DLL export/loading, registration and dispatch;
- conditional exclusions, disabled entrypoints, empty providers, unreachable
  implementations and incomplete initialization/teardown;
- original callers, transitive package-internal dependencies and necessary
  external boundaries, including callbacks and dynamically resolved calls;
- competing local implementations in mirror diffs, executable bindings,
  adapters, generated carriers and any retained overlay-derived code;
- intended repair, or a specific evidence-backed reason for retention or
  exclusion, with the affected capability and future retest condition.

Use separate fields for source presence, build/link selection, runtime
reachability and behavior verification. A green compile, an exported symbol
or a missing trace does not establish functional inclusion or exclusion.
No item may close as simply unknown, unused, optional or not needed.

Classify packages as fully connected, partially connected or disconnected,
while identifying legitimate load-only/tool/reference roles separately.
Quantify every category. Review previous exclusions on their evidence;
do not assume that prior task closure proves whole-package completeness.

## Repair approach

Recover missing original files and wrongly cropped bodies, then restore
complete owner-package composition: entry, initialization, service paths,
callbacks, failures and teardown. Resolve dependencies by original owner
first, through the smallest same-shaped modern binding where necessary.
An external original OpenNT dependency belongs in opennt-host only after its
required slice and finite outgoing boundary are established; do not import
an entire private Windows subsystem merely to satisfy a symbol.

Replace duplicate local behavior with the newly connected original provider
and remove superseded code and selections. Restore original formatting where
content is otherwise identical. Do not relocate autonomous code into a mirror
or hide it in generated output to claim reduction. Retain proven original
bug fixes and essential modern integration with explicit justification.

Work by complete owner package in dependency order, not one trace failure at
a time. Existing functioning capabilities are mandatory regression baselines,
not exemptions from this audit. Missing external hardware/services need
bounded mock coverage where feasible and explicit real-host limitations;
they must not be replaced with fake-success behavior.

## Planned S decomposition

| S | Scope | Required outcome |
| --- | --- | --- |
| S1 | Freeze upstream/repository/build baselines and perform full source-to-runtime reconciliation. | Complete package/path ledger, three connection categories, missing/unused/cropped worklist, duplication map and dependency-ordered repair design; every exclusion has evidence. |
| S2 | Restore missing or unjustifiably cropped original source and required source/build carriers. | Provenance-correct imports at original paths, resolved edition choices and role-correct manifests; importing files alone does not close runtime gaps. |
| S3 | Reconnect all unjustifiably unused or partially selected runtime owners, including lifecycle and dependent original slices. | Complete package-level behavior verification and removal of temporary bindings displaced by original code. At admission split this phase into sequential S packets, one complete owner package per packet, as the S1 findings require. |
| S4 | Consolidate mirror diffs, autonomous bodies and obsolete build/overlay selections displaced by this package's recovery. | Each displaced duplicate removed; retained recovered boundaries explained and tested; unrelated working-code minimization findings handed to the subsequent review with stable IDs. |
| S5 | Independently re-audit the entire source/build/runtime closure and run final integrated acceptance. | No unclassified source or unjustified runtime omission; reproducible artifact identities, footprint report and reviewed delivery. |

These are planned phases, not an allocation of active S identifiers. Any
renumbering after package-level decomposition must retain all obligations.
S1 may not silently defer difficult packages merely to finish the report.

## Verification and exit criteria

Every implementation S must pass the formal MSVC Win32/x86 build and all
previously accepted regressions at admission. At minimum include COMMAND,
MEM and EDIT directly, inside interactive COMMAND and through multiple
nested COMMAND levels, with actual guest text and expected exit behavior.
Include prior pipe/redirection, lifecycle and any accepted WOW workloads;
do not reduce the regression baseline to a few easy routes.

Each recovered capability requires normal, negative and teardown evidence
through its original caller/provider chain. Use link maps, symbol ownership,
registration and runtime observations together. Mock/unit results supplement,
but never replace, available real guest tests. Existing accepted guest
limitations remain recorded rather than repaired through guest mutation.

Final acceptance requires:

- every upstream package/path reconciled with actual source and composition;
- all recoverable omissions repaired, not merely proposed;
- serious/unavoidable exclusions explicitly justified with affected behavior,
  evidence, owner decision where required and a retest condition;
- no remaining duplicate local provider for a restored original owner;
- complete established regression, focused capability tests, documentation
  governance, diff checks and reviewed commit/push at delivery.

Report before/after file counts and added/deleted lines for mvdm and
opennt-host separately, both against pinned upstream and the task admission
revision. Separate new original imports from modifications to existing mirror
files. Report autonomous production bodies in adapters/executable bindings
and overlays separately, including both removed and added lines and net
change; account for moved/generated code without double-counting reductions.
Keep declarations, tests, documentation and build tooling in separate totals.
Explain every necessary increase. The objective is a smaller non-original
footprint, never a lower line count obtained by losing working capability.

## Relationship to other queued work

The subsequent [implementation minimization review](proposal-original-source-implementation-minimization-review-001.md)
starts from local implementations and seeks their original replacements.
This candidate starts from the complete original MVDM universe and checks
for omissions and unused originals, including gaps with no local substitute.
Reuse and verify prior recovery results; repair residual gaps rather than
duplicate completed work. This candidate precedes implementation minimization
so that the latter can consolidate against the recovered original closure.
It does not divert the active runtime-completion task.

The queue places this package after contract auditing, WOW completion,
error-response recovery and product experience repair. Its repair ownership
is missing/cropped/unconnected original capability, including removal of local
providers displaced by that restoration; unrelated working-code minimization
belongs to the subsequent review. Maintain shared stable finding/source-owner
IDs, source/build hashes, dispositions and executable evidence. Pass that
reconciled inventory forward, including changed and unresolved rows; unchanged
accepted evidence is reopened only for changed inputs or counterevidence.
Full coverage reconciliation is required, but repeating all prior source
research is not. Neither package defers cleanup necessary for its own repairs.

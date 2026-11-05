# Proposal — implement the existing DOS16 and WOW32 audit inventories

## 2026-10-31 owner-directed handoff

This first queued candidate inherits unfinished T473 original S11 execution,
scheduling, callback, identity and lifetime recovery, and its former S13–S21
scopes, including the separately described Winmine geometry/background issue,
DOS, devices, WOW, USER, GDI, minimization and final capability verification.
The [T473 stage plan](../etc/operations/m0-t473-whole-recovery-stage-plan.md),
[recovery worklist](../../artifacts/mvdm-audit/recovery-worklist.md) and
linked evidence are the handoff inventory. At admission reconcile each item
against completed delivery and current behavior, and establish bounded stages
without dropping or duplicating work. T473 S12 remains with T473 and is not
transferred. Preserve selected CCPU and its V86 emulation; do not restore the
retired hardware V86 monitor. Reuse original code only where its role and
selected architecture are established; preserve guest immutability and the
owner's boundary-only scope. This transfer does not admit the candidate.

## Objective and admission

Owner-requested candidate, ordered before the two independent reviews:
implement and validate the actionable findings and handoffs in the existing
DOS16 and WOW32 audit inventories, together with inherited T473 work. Recover
the original owners those inventories call for, preserve their exclusion
decisions, minimize related mirror diffs and remove displaced autonomous
implementations. This is an implementation task, not a repeat source audit.

This proposal does not allocate a numeric T, admit implementation, or change
the current active packet. Assign its T number through CURRENT.md when the
owner admits the candidate. Freeze the then-current baseline: earlier queue
work may already have repaired some gaps.

## Governing principle

Maximize justified original-owner reuse while excluding original parts that
do not belong in the selected product. Inclusion and exclusion both require
an explicit source role, selected architecture and dependency-boundary
decision. Original existence alone does not establish product eligibility;
convenience, lack of a current trace hit, unresolved symbols, or a few
working applications alone do not establish a valid exclusion. Apply existing
source-first and package-closure rules, including their zero-degree and
transitive-closure requirements, and record any required owner decision.

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

## Execution objectives, boundaries and method

### Owner correction: execute the existing audit inventories

The primary execution inputs are the owner's two existing, read-only audits,
not a newly commissioned original-source census:

- DOS16/non-WOW host: `O:/repos.hobby/ntvdm64-dos16-audit/management/README.md`,
  its `seam-scope.json`, `seam-work.json`, `seam-records.json`, source evidence,
  module reports and handoffs.
- WOW32: `O:/repos.hobby/ntvdm64-wow32-audit/management/README.md`,
  `management/SEAM-SCOPE.md`, `management/seams.csv`,
  `management/seam-sites.csv`, `management/seam-gaps.csv` and
  `management/IMPLEMENTATION-SEAMS.md`, with their linked evidence.

At admission query each audit's current authoritative management entrypoint,
pin the revisions and input identities, and map its actual entries and
handoffs to the current product, prior delivered T473 work and executable
verification. Preserve every applicable row and existing scope boundary;
make actionable, already-delivered, excluded, pending-audit and unresolved
states explicit. Implement the existing actionable inventory in dependency
order; inspect additional source only to resolve a specific missing, stale,
contradicted or out-of-scope decision, not as a replacement global audit.
If an audit is still open or its static evidence is incomplete, preserve that
qualification and record its affected rows rather than inventing completed
conclusions. Static audit closure is not proof of product integration or
runtime behavior. These audit repositories are read-only comparison material,
never product source, build, link, runtime or acceptance dependencies.

**Hard exclusion:** never restore the retired hardware V86/CPU30 monitor into
the product's source selection, build, link or runtime. The selected CCPU40
and its software V86 emulation remain required: those are distinct from the
retired hardware monitor. Before each source/build selection, check the audit
disposition, product architecture and transitive dependencies; a proposal to
cross an established exclusion stops for explicit owner-directed governance,
not an inference from original-code presence or a build failure. Verify the
actual build graph, linked artifacts and selection manifest at each affected
delivery. Record any contrary evidence without claiming the current product
contains the retired monitor until that is separately verified.

The shared owner goals for this candidate and the next two candidates are:
(1) maximize justified original integration while definitively excluding
ineligible parts; and (2) minimize mirror differences and autonomous
implementation without losing verified capabilities. This candidate executes
the existing audited dispositions and inherited T473 handoffs, including
recovery of justified missing/cropped/unconnected owners and removal of
local code displaced by that recovery. It is not an
authorization to link all historical code, guest material, alternate CPU
profiles, private host subsystems or incompatible editions.

Use each applicable audit row's established source role, consumers,
architecture and dependency decision as the implementation starting point;
check its applicability to the current build and handle a concrete conflict
through the applicable governance rather than re-auditing every source unit.
For an admitted owner, attempt original composition, then
the smallest same-shaped binding, and only then a registered exception under
the source-first rules. Verify actual compilation/link/dispatch and behavior
separately. Restore and test the complete affected owner, including failure
and teardown, and remove displaced duplicate providers in the same stage.

Maintain one shared inventory with stable finding and original-owner IDs,
pinned source/build identities, admission or exclusion rationale, mirror diff,
local implementation, disposition, and evidence links. Hand it to both
subsequent independent reviews. Those reviews may challenge and correct any
decision; reusing evidence never substitutes for independent assessment.

## Implementation scope and tracking

Start with the two existing audit inventories and their pinned canonical
OpenNT/OpenNT-4.5 source identities, not a fresh global discovery pass or only
the current build manifest. Retain edition conflict decisions and source
identities; do not silently combine incompatible alternatives. Reconcile the
audited rows and prior T473 delivery against actual files and build artifacts.

For each applicable audit entry and inherited handoff, track its stable ID,
audited disposition, current implementation status, responsible owner,
required change, applicable dependency/exclusion, delivery and verification
evidence. Preserve pending or unresolved audit entries as such. Distinguish
source presence, actual build/link selection and runtime behavior where needed
to establish implementation; neither a file's presence nor static audit
closure establishes execution. Do not repeat a package-wide or file-by-file
source audit, create a new restored-code percentage census, or re-decide every
established exclusion as a prerequisite to implementation. All applicable
actionable rows require implementation and the prescribed delivery gates;
guest immutability and approved host-semantic boundaries remain in force.

## Repair approach

Recover missing original files and wrongly cropped bodies, then restore
complete owner-package composition: entry, initialization, service paths,
callbacks, failures and teardown. Resolve dependencies by original owner
first, through the smallest same-shaped modern binding where necessary.
An external original OpenNT dependency belongs in non-mvdm only after its
required slice and finite outgoing boundary are established; do not import
an entire private Windows subsystem merely to satisfy a symbol.

Replace duplicate local behavior with the newly connected original provider
and remove superseded code and selections. Restore original formatting where
content is otherwise identical. Do not relocate autonomous code into a mirror
or hide it in generated output to claim reduction. Retain proven original
bug fixes and essential modern integration with explicit justification.

Work by complete owner package in dependency order, not one trace failure at
a time. Existing functioning capabilities are mandatory regression baselines,
not exemptions from implementation verification. Missing external hardware/services need
bounded mock coverage where feasible and explicit real-host limitations;
they must not be replaced with fake-success behavior.

## Planned S decomposition

The owner directs one S per unresolved host finding, in this priority order:
(1) original code that can replace a duplicate local implementation;
(2) missing original integration without a local equivalent; and (3) known
problems and partially completed fixes. The detailed numbered mapping and
per-item gates are in the [T478 finding-stage plan](../etc/operations/m0-t478-finding-stage-plan.md).
CURRENT.md controls the sole active packet; future S numbers are a plan, not admission.
No item in the 69-row report yet proves priority (1), so none is invented.
The 40 planned S2–S41 packets cover applicable candidates from that report;
S1 must still reconcile the larger existing inventories and inherited
handoffs and incorporate any further applicable findings. Guest source,
media, tools and packaging are outside scope; guest material is read-only
supporting evidence of an identifiable mvdm-host gap only.

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

- every applicable audited package/path and inherited handoff reconciled with
  actual source and composition, with audit gaps and limitations explicit;
- all justified and recoverable omissions repaired, not merely proposed;
- every exclusion explicitly justified by its role, selected architecture and
  applicable rules, with affected behavior,
  evidence, owner decision where required and a retest condition;
- no remaining duplicate local provider for a restored original owner;
- complete established regression, focused capability tests, documentation
  governance, diff checks and reviewed commit/push at delivery.

Report before/after file counts and added/deleted lines for mvdm-host and
non-mvdm separately, both against pinned upstream and the task admission
revision. Separate new original imports from modifications to existing mirror
files. Report autonomous production bodies in adapters/executable bindings
and overlays separately, including both removed and added lines and net
change; account for moved/generated code without double-counting reductions.
Keep declarations, tests, documentation and build tooling in separate totals.
Explain every necessary increase. The objective is a smaller non-original
footprint, never a lower line count obtained by losing working capability.

## Relationship to other queued work

The subsequent [mirror-diff review](proposal-unconnected-mirror-diff-cleanup-001.md)
independently evaluates every mirror difference, including connected and
unconnected changes. The [project-wide implementation review](proposal-original-source-implementation-minimization-review-001.md)
independently evaluates autonomous implementations and original-owner choices.
Both may challenge the existing audit and this task's inclusion, exclusion or retention
decisions and implement corrections within their scopes. They reuse pinned
evidence when still valid but independently examine the actual conclusion and
code; changed inputs or counterevidence require renewed verification. Neither
later review excuses leaving recovery-related cleanup unfinished here.

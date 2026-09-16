# Mirror, overlay and adapter footprint cleanup

## Objective and admission

Owner requested this candidate first in Queue after the read-only audit:
reduce unnecessary mirror differences and overlay/adapter autonomous code,
split the work into S packets, and quantify all three reductions at acceptance.
This proposal does not allocate a numeric T or admit source changes. Use
Ordinary Mode and sequential S packets upon admission.

Restore original source where composable and delete obsolete compatibility
paths. Moving code between directories is not net reduction or source recovery.

## Bounded workset and starting observations

Scope: audited startup, diagnostic, declaration and private composition seams
in mvdm-host/opennt-host, their overlays and directly associated adapter,
session, build and test code. Include same-pattern instances in those families;
do not restart global source BFS or absorb downstream x87/WOW recovery.

| Group | Starting observation and required disposition |
| --- | --- |
| WOW startup fallback | cmdmisc.c still supplements missing OS -a from session state. run16 now uses original BaseGetVdmConfigInfo and CreateProcess; the setter had no caller in src/tests/tools. Prove the complete graph, then remove the obsolete fallback, adapter getter and session storage/accessors together. |
| Diagnostic hooks | cmddisp.c +29/-0 and emm_fncs.c +13/-0 were diagnostic-related, not 42 guaranteed deletable lines. Audit consumers and adjacent nt_eoi, nt_mouse, nt_timer and ntvdm observations before restoration. |
| EOI bridge | Two files / 28 physical lines. Consider a minimal typed LONG temporary, original call and write-back at the original owner; preserve ABI, outputs and caller/null contracts, not unsafe pointer casts. |
| CCPU state and SAS wrapper | localfm.c 20 lines and sas_overwrite_memory.c 16 lines. Try original translation-unit/subset composition without duplicate Sas or conflicting globals. |
| C-VID binder | Binder/header 89 lines plus generated bindings. Preserve full vector layout and original typed accessors; CPU/video policy is not adapter policy. |
| BaseSrv pending query | dos_worker_wait.inc 29 lines. Record lookup and locking stay BaseSrv-owned; preserve resident-worker versus pending-GetNextVDMCommand distinction. |
| Declarations | nt_sound.c +6/-0 is an initial candidate. Prove original declaration ownership and compile ABI; distinguish relocation from eliminated behavior. |

Initial hand-written overlay inventory: seven code files / 182 physical lines
(MVDM 153, OpenNT 29), excluding README and generated bindings. The earlier
125-line CPU/CVID cohort is only a subset. S1 must remeasure; these are planning
observations, not promised savings or the final admission baseline.

## Planned S packets

### S1 - Freeze provenance, ownership and measurement

Freeze repository commit, upstream paths/hashes, actual formal graph and exact
workset. Classify each row as restore/delete, shrink, retain or modern-boundary
migration with original owner, callers, source-first alternatives and tests.
Sweep stale three-in-one startup paths, diagnostics, declaration wrappers and
x64-only patterns within the workset; a compatibility label alone proves nothing.

Exit: repeatable before totals, complete candidate ledger and coherent repair
groups. Unproved removals remain candidates assigned to the following S review.

### S2 - Remove the obsolete WOW startup chain

Verify shared/separate WOW worker OS command-line generation and consumption,
quoted/spaced kernel paths, missing/malformed inputs and original error behavior.
Restore original cmdmisc behavior and remove the superseded complete carrier
only after confirming no selected caller depends on it. Preserve DOS direct
launch, nested COMMAND re-entry and the product CLI.

Exit: no redundant carrier for the restored path, positive/negative contract
tests, formal x86 build and DOS regression. Compare WRITE startup depth with S1;
this is not a claim of complete WOW compatibility.

### S3 - Simplify private overlay composition

Process EOI, localfm/SAS, C-VID and the BaseSrv query as coherent ownership groups,
not one task per tiny edit. Attempt original composition first, delete redundant
wrappers/build/header layers and justify every retained subset by concrete
dependencies. Migrate only proven modern OS mechanics to the named adapter;
never CPU/vector/record policy. Respect mirror insertion limits and private
overlay access: external components/tests cannot directly call an overlay.

Exit: every overlay file and generated family has a disposition; ABI,
single-definition/vector-layout tests and resident-versus-waiting broker
regression pass. Compile and run relevant integration after each coherent group.

### S4 - Retire completed observations and consolidate declarations

Remove obsolete hooks plus unreferenced adapter support, configuration and test
dependencies as complete chains. Preserve owner-required trace for autonomous
semantic repairs and failures. Relocate necessary observation to an existing
proper boundary only with equivalent evidence; default-off code still counts.
Recover declaration-only diffs through the smallest original-shaped binding,
not copied headers or unsafe casts.

Exit: required trace coverage remains testable; removed paths have no references
or build inputs; focused tests, formal build and regression pass. Separate
behavior deletion from comment/declaration relocation in the report.

### S5 - Integrated verification and quantified closure

Fresh MSVC x86 /MT CCPU40 build of run16.exe, basesrv.exe and ntvdm.exe. Verify
COMMAND/MEM, nested COMMAND child execution/return, EDIT keyboard/mouse,
EDIT-exit then MEM output, broker pending-command/exit behavior, and affected
existing IP/exception/C-VID fixtures. Exercise WRITE startup and compare exact
reached behavior with S1: known unsupported WOW behavior is neither a passing
full-WRITE result nor permission to skip the startup regression. Verify required
trace hits and record unavailable checks as limitations.

Publish only tested products to O:\winnt with hashes. Build outputs remain under
build/<task-id>/<run-id>; runtime logs remain under O:\winnt\logs. Recompute the
metrics below, review final diff and divergence registers, run governance/link
checks, commit/push and leave a clean synchronized worktree. Owner visual
acceptance remains distinct from automated evidence.

## Mandatory final acceptance metrics

Use identical upstream identities and counting rules before/after. Report each
S and the full T; distinguish the frozen cohort from broader root totals. Count
the union of before/after paths, including removed/new files; never shrink the
cohort or change source edition to manufacture reductions.

| Metric | Required report |
| --- | --- |
| Mirror diff | Separately for mvdm-host/opennt-host: changed files, added A/deleted D lines, distance A+D, before-minus-after reduction, exact-restored files and remaining reasons. Use a fixed diff algorithm with only line-ending normalization and retain original hashes. |
| Overlay | Per owner: files and physical source/header lines before/after, additions/deletions/net reduction; classify original-derived versus autonomous portions. Generated bindings and their generator/source cost require a separate reproducible row, not exclusion from footprint accounting. |
| Adapter autonomous code | Separately for adapter-mvdm-host-in/out and adapter-opennt-host: project-authored implementation files, functions and physical lines under a fixed S1 classification; additions/deletions/net reduction and retained-mechanic reasons. Do not classify all adapter code as invented algorithms. |
| Displacement guard | Include related app/session/broker/patch/build-helper changes and code moved into mirrors or other roots. List source, destination and line count for each relocation: moving code alone eliminates zero net code. |

Identify comments, blanks and declaration-only changes separately from runtime
behavior. Any executable-line metric needs a fixed reproducible method. Report
increases and zero reductions honestly; disabled code is not deleted code.

The final Chinese owner summary must explicitly state how many mirror diff lines,
overlay lines/files, and adapter autonomous lines/functions were reduced, which
changes merely moved code, and which necessary seams remain because original
code cannot replace them. Link reproducible before/after evidence and tests.

## Boundaries and exit criteria

Preserve exception-scope repairs, full USE16 IP arithmetic, C-VID layout,
guest-memory leases, mouse/BOP semantics and broker failure rules. No CPU30/x64
product, kernel/CSRSS reconstruction, generic adapter or retired one-process
startup. x87, selective NTVDMx64 corrections, WOW/debugger and full WRITE remain
their existing candidates' responsibility.

Every scoped row ends in tested restoration/removal or evidence-backed retention;
unexamined items cannot close. Mandatory work needs an explicit disposition,
not silent deferral. New semantic repairs or changed boundaries need admission.
Correctness overrides an arbitrary savings quota: closure requires measured
results, justified residuals, no regression and clean committed/pushed delivery.

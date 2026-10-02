# Proposal — NTMON Console-root worker tree

## Status and dependency

Owner-approved planning dated 2026-10-02, unnumbered candidate at the head of
the remaining Queue. The active component-renaming/lifecycle packet is not
expanded or displaced. Admit this candidate only after that package delivers
its coherent baseline. Names here use the final identities: NTCON is the
visible Console/Window frontend (previously NTKVM), NTW32 is the native text
worker, and NTVDM hosts DOS/Win16.

Replace NTMON's flat worker list with a tree of registered Console roots and
their associated workers. Include Console roots in management selection and
DEL actions. This is an authenticated NTSRV management projection, not a
process ancestry tree or the later Direct/Observed task-trace feature.
The [task-trace candidate](proposal-worker-task-trace-observation-001.md)
reuses this ordinary management view and owns only its separate detail modal.

## Approved UI

```text
                         NTVDM Task Monitor

   PID     KIND      STATE    ELAPSED   STACK  TASK
--------------------------------------------------------------------------
   38448   CONSOLE   BUSY     00:05:12
     32252 DOS       BUSY     00:04:58      2  O:\WINNT\COMMAND.COM
     38268 Win32     IDLE     00:02:31      0  <EMPTY>

   45100   CONSOLE   MISSING  --:--:--
     46208 DOS       BUSY     00:01:42      1  O:\WINNT\EDIT.COM

   50812   CONSOLE   IDLE     00:01:06
     51904 DOS       IDLE     00:01:02      0  <EMPTY>

   -       UNBOUND
     41720 Win16     BUSY     00:03:06      1  O:\WINNT\WINMINE.EXE
--------------------------------------------------------------------------
UP/DOWN=Select Task    DEL=End Task    ESC=EXIT
```

CONSOLE roots are unindented, their workers indented. KIND for workers remains
DOS, Win16 and Win32, with existing worker values 0/1/2 unchanged. CONSOLE is
a management node category, not a fourth worker kind. ELAPSED follows STATE
and measures the actual live process's age, not task age or last repaint time.
Missing roots show --:--:--; no invented live elapsed counter.

UNBOUND is the final group in the list, not a data column or a synthetic
Console process. It contains genuinely unassociated workers; its heading has
no process identity, lifecycle or aggregate state. DEL on the heading does
nothing; its worker rows remain independently selectable/manageable.

## State semantics

- Live Console roots display BUSY while an associated worker owns interaction
  or an admitted task/handoff is active. IDLE means no active program or
  interaction remains, associated workers are idle and the root can accept
  another request. A temporarily blocked parent is not idle merely because it
  has no immediate frame or input traffic. Pending startup/handoff must not
  produce a false ready-to-accept IDLE display.
- Live workers display BUSY while serving active work, including legitimate
  waits; IDLE represents the existing resident idle/empty condition with no
  running task, retaining STACK=0 and TASK=<EMPTY> where appropriate. Map
  source states by their actual meaning, not by a blanket READY->IDLE rename.
  Failed/disconnected/unknown transitional data must remain truthful rather
  than being labelled IDLE and advertised ready.
- These labels are display projections only. They do not rewrite original
  DOS/WOW records, GetNextCommand, admission, completion, READY/BUSY/EMPTY
  lifecycle or native Console resource-safety predicates. Do not use later
  Observed records, timer sampling or traffic activity to decide these labels.
- A missing former Console is MISSING, not IDLE. Its associated workers
  retain their actual reported state while they still exist. Existing broker
  shutdown decisions continue independently; the UI must not keep them alive.

## Authoritative tree and missing roots

NTSRV owns Console/frontend registration, generation-safe identity, worker
association, liveness and copied management snapshot. NTMON only reads that
snapshot; it never enumerates host processes, inspects Console membership or
contacts workers to reconstruct a tree. NTCON owns presentation, not this
management registry. run16 and worker-base do not acquire management policy.
Cross-EXE declarations and RPC changes belong to src/interface.

Snapshot rows must distinguish live Console, retained missing Console, worker
and UNBOUND heading without repurposing DOS/Win16/Win32 kind values. Include
stable root/worker generation identities and association; a displayed PID alone
cannot authorize selection, grouping or deletion. Snapshot copying and root
death/worker unregistration need a consistent lock boundary and versioning.

When a root disappears, retain only the minimal identity/tombstone necessary
for workers still associated with that exact root generation. Display its old
PID and MISSING, with surviving workers below it. Do not move those workers
to UNBOUND merely because their known former root died. Remove the missing
root when its last association disappears. PID reuse or a new frontend root
must not inherit the previous root's children; rebind only on an actual
authenticated NTSRV association transition. Do not retain an unbounded history
or another mutable execution graph.

WOW's current path does not request a character frontend lease and excludes
shared WOW from worker/frontend capability binding. It belongs in UNBOUND
unless the then-current implementation proves a real association. Launching
Win16 from a Console is not enough to create a lifecycle parent. Source-only
launch provenance is outside this tree; do not fabricate it or close WOW when
an unrelated launching Console closes.

## Selection and DEL

- Preserve the title NTVDM Task Monitor, footer text, UP/DOWN navigation,
  DEL management action and ESC exit. Do not change these hotkeys incidentally.
- Live worker DEL uses the existing authenticated NTSRV worker-close request
  and acknowledgment, preserving actual Console-session shutdown semantics.
- Live CONSOLE DEL sends an authenticated request to NTSRV for that exact
  registered frontend root. NTSRV requests the existing orderly frontend
  shutdown; any associated worker retirement follows existing lifecycle rules.
  NTMON does not issue a recursive kill or invent a new termination policy.
- DEL on MISSING or UNBOUND heading is a harmless no-op. Pending close
  shows its truthful result; never report success just because a row vanished
  or one pipe disconnected. Rapid refresh/PID reuse cannot target a replacement.
- Retain selection by stable node identity across refresh; if removed, select
  a deterministic nearby row. Independent roots and unbound WOW stay isolated.

## Proposed S sequence

| S | Complete result and acceptance gate |
| --- | --- |
| S1 | Audit current registration, real worker/root associations, state projection and existing close acknowledgments; freeze node DTO, lock/identity/tombstone ownership and exact fixture matrix. Source-confirm shared WOW exclusion and distinguish genuine IDLE from waiting/busy states. |
| S2 | NTSRV copied root/worker snapshot plus bounded missing-root retention and authenticated frontend-close operation; interface version/RPC synchronization and malformed/stale/cross-session tests. Production consumers wired with no second registry or changed lifecycle policy. |
| S3 | NTMON tree, indentation, CONSOLE/BUSY/IDLE/ELAPSED columns, final UNBOUND group, stable selection and correct DEL routing/no-ops. Test layout, refresh, missing roots, mixed worker kinds and existing keyboard controls. |
| S4 | Actual multi-root management and lifecycle integration, root deletion with surviving-worker intermediate snapshot, eventual tombstone removal, idle/reuse and WOW independence; full product regression/publication, semantic/diff audit and owner acceptance handoff. |

Each implementation S maintains its own capability checklist and exact test
entrypoint/assertion/evidence links, connects completed work to production and
passes the existing code-bearing P build, runtime, coherent publication and
commit/push requirements. Do not wait for the later task-trace package or hook
implementation to make this ordinary management tree usable.

## Verification and exclusions

Cover two independent Consoles, several DOS/native workers on one root,
BUSY->IDLE->BUSY reuse, legitimate blocked execution, pending startup, Console
with no workers during its bounded startup admission, frontend/process/broker death,
live root DEL, close failure, worker DEL, stale snapshot/action, generation/PID
reuse, missing root with surviving children and final removal. Use controlled
fixtures for brief missing-root states if real cooperative shutdown finishes
too quickly; never alter product shutdown to manufacture the display case.
UNBOUND heading DEL must send no RPC; deleting a bound Console must not affect
unbound WOW. Verify title, exact footer, cursor/selection stability and columns
in visible Console and Window. Retain 17 Console + 17 Window routes and existing
WOW frontiers, x86 build, focused lifecycle/protocol/monitor tests and coherent
runtime-package publication to O:/winnt under the then-current naming manifest.

No Job observation, launch hook, descendant graph, new scheduler/helper,
guest/shared-library change, process enumeration, task completion source or
worker lifecycle redesign is admitted. This planning change introduces no
production code or passing runtime claim; numeric admission remains in CURRENT.

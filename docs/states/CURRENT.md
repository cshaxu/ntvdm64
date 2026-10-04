# Project Status

## Current Work

**Active: M0 T426 S1** (Ordinary Mode).

Owner closes T425 and directs admission of the next queued T. T425 closure is
delivered and pushed at 8cbc1c997; [closure](../history/m0-t425-worker-neutral-frontend-closure.md)
and [S11 evidence](../etc/evidence/m0-t425-s11-typeahead-closure.md) retain the
actual proof and limits. T426 consumes the former queue-head
[Console-root worker-tree proposal](../proposals/proposal-ntmon-console-worker-tree-001.md).
Only S1 is admitted: inspect current service registration/projection/close
boundaries and freeze the implementation contract before changing production.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T426 S1; Ordinary Mode. |
| Admission And Approval | Owner: “一起纳入提交。然后准入下一个T”. T425 is closed at 8cbc1c997; admit the former queue head, with one active bounded S. |
| Candidate Proposal | [NTMON Console-root worker tree](../proposals/proposal-ntmon-console-worker-tree-001.md). |
| Objective | Produce a source-backed Console-root/worker snapshot and management contract, source ownership/lock inventory, stable identity/tombstone rules and exact implementation/test handoff for the subsequent bounded stages. |
| Non-goals | No production implementation in S1, descendant observation, Job/hooks, synthetic tasks, new scheduler/helper, guest/library changes or worker execution/lifecycle redesign. No claim that the tree UI already works. |
| Reference Baseline | Delivered T425 8cbc1c997 and unchanged S9/r022 eight-file package; current NTCON frontend, NTVWM native worker, NTSRV authority and NTMON snapshot consumer. |
| Files And ABI Surface | Audit src/ntsrv-exe, src/ntmon-exe and their current callers; protocol declarations belong to src/common/protocol, not retired src/interface. Document the smallest proposed DTO/RPC/version changes without implementing them. S1 owns CURRENT and indexed audit/design evidence only. |
| Applicable Rules | docs/README.md reading set, Execution/Architecture/Coding/Document rules, design authorities, CONTRIBUTING and source policy. Original DOS/WOW ownership and completion remain unchanged. |
| Verification | Read actual production callers/providers and management fixtures; review identity, locking, projection and close acknowledgement contracts. Documentation governance with links, actual diff review and git diff --check. No runtime/build capability claims from an audit-only P. |
| Expected Markers | Each root/worker/state/close requirement maps to a source owner, proposed contract and exact positive/negative test; gaps and unsupported WOW/task identities are explicit. |
| Asset Needs | Current repository source and retained T425 build/test evidence; no new guest media, process, helper or desktop interaction needed. |
| Reporting Requirements | Report current versus proposed behavior, retained adapters, lock/resource owners, exact downstream tests, limits and bounded S2-S4 implementation sequence. |
| Stop Conditions | Required original execution change, incompatible ownership, absent trustworthy identity/close contract or material scope expansion requires re-admission; do not invent process-tree control. |
| Exit Criteria | Source-backed contract and test inventory complete, all proposal requirements dispositioned, governed evidence reviewed/committed/pushed; implementation remains unclaimed until its own stages pass. |
| Original Owner Request | Include reviewed planning changes in T425 commit, then admit the next T. Its approved candidate is NTMON Console-root worker tree, followed by existing Queue order. |
| Similar-Issue Sweep | Root rebuild/PID reuse, missing-root retention, independent Consoles, worker/GUI/WOW association, stale selection and authorization, orderly close failure, UNBOUND deletion, unchanged labels/title/three hotkeys and elapsed-time meaning. |

T426 sequence follows the approved candidate: S1 contract audit; subsequent
bounded admission for NTSRV projection/close and protocol negatives, NTMON
tree/selection UI, then integration/publication and owner handoff. Preserve
DOS=0 / Win16=1 / Win32=2, title NTVDM Task Monitor and the original three
hotkeys. CONSOLE is a frontend row, not a fourth worker kind. NTMON never
enumerates processes or reconstructs authoritative associations.

## Current Technical Baseline

Published O:/winnt still equals build/M0-T425/S9/r022/runtime, all eight hashes
verified during S11. Files: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe,
ntvwm.exe, ntmon.exe, WOW32.DLL and VDMREDIR.DLL. Recovery/publication manifest:
S9/r030; published smoke: r031. MSVC14.43 / SDK22621 / Win32 x86 /MT CCPU40,
APP0.0.425, control/RPC37 and I/O25.

S9/r035-r036 independently passed Console17/Window17 and the three retained
WOW frontiers. S11/r006 adds four selected real continuous-input cases with
zero consumption waits, expected root exit 1, complete MEM reports and
identity-checked cleanup. Z: is removed; no owned product process remains.
S11 changes only tests/docs, not C build or production inputs; valid sealed
builds and matrices are reused by identity. No product replacement is needed.

S8 accepted software VGA FULLSCREEN, natural mouse route/draw/erase and
producer-side update triggers remain. NTCON/worker-base do not filter emitted
display events. Original guest/device/execution logic and imported libraries
remain at their owners. Physical RDP and broader WOW usability retain their
explicit prior boundaries. No speculative CAF repair or new helper/channel/
scheduler is introduced.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T425 | Owner-directed closure after S11 typeahead proof; published S9/r022 unchanged. [Closure](../history/m0-t425-worker-neutral-frontend-closure.md). |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone. [Closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

T425 closure commit 8cbc1c997 includes the owner-approved other-session Queue
and two proposals without expanding T425 implementation. This separate
documentation-only admission allocates T426 and removes its candidate from
[Queue](QUEUE.md), retaining every remaining candidate's relative order.
S1 is audit-only; no production build, runtime mutation or redeployment is
required for admission. Documentation governance, link/diff review and clean
synchronized Git delivery apply. Outstanding debt remains in [TODO](TODO.md).

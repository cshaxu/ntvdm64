# Project Status

## Current Work

**No active M/T/S packet.**

Owner accepts T427 on 2026-10-04: “本t可以收口了。” The
[closure record](../history/m0-t427-system-root-search-isolation-closure.md)
retains S1-S5 deliveries, verification, publication and limits. No next task
is admitted. Candidates retain [Queue](QUEUE.md) order; debt remains in
[TODO](TODO.md). Implementation waits for owner admission.

## Current Technical Baseline

Production delivery 62a71fd90, followed by pushed registration cc245ca95,
is the T427 S5 system32 host layout. MSVC14.43/SDK22621/Win32 x86 /MT
CCPU40; APP0.0.427/RPC38/I/O25. O:/winnt/system32 contains all six EXEs
and both host DLLs. Each loaded EXE's parent defines the product Windows root.
Original runtime media use declared system32 locations; root SYSTEM.INI and
beside-worker NTVDM.REG ownership remain. User search is ordinary CWD/PATH
or explicit paths. Native host APIs and Win16 directory roles remain distinct.

[S5 evidence](../etc/evidence/m0-t427-s5-system32-host-colocation.md) records
affected builds, focused tests, r012 Console17/Window17/WOW, recoverable r011
publication and r013 published smoke/hash checks. The accepted eight-file set
equals build/M0-T427/S5/r001/runtime. This documentation-only closure changes
no production input and does not rebuild, redeploy or stop running programs.

Owner has placed extra Windows 3.1 WINMINE/SOL/WRITE applications at
O:/winnt. Same-name NT4 setup entries do not establish these binaries'
provenance. They remain user applications. Sealed WOW results retain their
tested inputs, not a new test claim after the owner's relocation. Physical
RDP/focus, broader WOW usability and scrollback retain disclosed limits.

Other sessions' queue/proposal edits are preserved outside this closure.
No next candidate is silently admitted or renumbered.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T427 | Owner-accepted root/search isolation and system32 host co-location; S5 delivery 62a71fd90. [Closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree, S4 delivery 700b3d862. [Closure](../history/m0-t426-console-root-monitor-tree-closure.md). |
| T425 | Owner-directed closure after S11 typeahead proof. [Closure](../history/m0-t425-worker-neutral-frontend-closure.md). |
| T424 | Owner-closed at audited ff50ff588, production S12 46abdb554. [Closure](../history/m0-t424-component-control-closure.md). |
| T423 | Owner-accepted S40 f64559086. [Closure](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone. [Closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

Owner accepts T427 on 2026-10-04. This documentation-only closure moves its
closed chronology to history, preserves indexed S evidence and unrelated
planning edits, and leaves no active packet. Governance, relative-link and
diff checks apply. Next admission requires owner direction.

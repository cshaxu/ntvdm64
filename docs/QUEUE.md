# Queue

This is the authoritative ordered queue of **unnumbered candidate T packages**.
A candidate receives its next global `T` number only when the owner admits it
to [STATUS.md](STATUS.md). The active Base VDM package is deliberately absent:
its live scope and progress belong only to Status.

Recovery is package-first and mirror-first: a trace, BOP hit, compiler error
or convenient API may verify a completed package but cannot choose the next
package or substitute a new provider for source-shaped recovery. The first
twelve recovery packages are implementation packages, not repeated global
audits. Each performs only the bounded immediate-boundary audit needed to
recover its original code, then implements and locally verifies its whole
owner slice. Cross-package BFS, interface and perimeter ledgers remain shared
evidence, not a serial implementation blocker.

The detailed proposal, predecessor, delivery and exclusions for every queue
item are in the [package recovery proposal set](etc/operations/package-recovery-program-20260827.md).

| Order | Candidate package |
| --- | --- | --- |
| 1 | Bochs/SoftPC mechanical contract closure |
| 2 | MVDM ingress and fixed-width event bridge |
| 3 | DEM/COMMAND vertical slice and child lifecycle |
| 4 | Minimal DOS guest execution and controlled return |
| 5 | Minimal vertical-slice integration closure |
| 6 | XMS owner package |
| 7 | DPMI/DPMI32 owner package |
| 8 | Redirector owner package |
| 9 | WOW32/Win16 owner package |
| 10 | VDD owner package |
| 11 | Debugger/BDE/FAX owner packages |
| 12 | Service-package cross-family closure |
| 13 | Broker process and cross-process coordination |
| 14 | In-process multi-session reentrancy audit |
| 15 | Host capability expansion |
| 16 | Guest DOS/WOW16 artifact matrix |
| 17 | Product release closure |

Historical queue/proposal material is indexed in
[the pre-package-roadmap archive](etc/archive/2026-08-27-pre-package-roadmap/README.md).

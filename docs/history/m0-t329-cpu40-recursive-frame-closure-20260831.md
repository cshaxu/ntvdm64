# M0 T329 — CPU40 recursive-frame lifecycle closure

T329 closed the complete selected original CPU40 recursive simulation-frame
package without changing runtime behavior.

The selected original `c_cpu_init` establishes main-thread TLS before the
first CPU entry. Original `c_cpu_simulate`, `ccpu386SimulatePtr`, the exception
frame, BOP-FE `c_cpu_unsimulate` and `ccpu386Unsimulate` share the same
per-thread frame stack. All selected direct re-entry callers use that same
route; unselected WOW callers stay with the future WOW package.

The original level-zero `ccpu386Unsimulate` behavior is identical in all three
approved OpenNT baselines and was not altered. Current x86 and x64 bounded
original recursive execution reaches `returned-recursive` successfully.

The persistent fixed-container failure therefore needs actual exception
context, not a speculative frame guard. T330 owns copied, report-gated context
attribution at the original exception boundary.

Evidence: [T329 S1 lifecycle contract](../etc/evidence/m0-t329-s1-original-cpu40-recursive-frame-lifecycle-contract-001.md).

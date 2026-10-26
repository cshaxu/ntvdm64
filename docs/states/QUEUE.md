# Queue

This is the authoritative ordered queue of unnumbered candidate packages.
Only [Current](CURRENT.md) admits an active task. The lifecycle and WOW32
execution-continuation packages and USER objects/shared view
are owner-closed. Remaining candidates retain their relative scope; the table
ordinal is recomputed for the remaining rows. The [WOW32 program](../proposals/proposal-wow32-production-completion-002.md)
and its approved cross-package mappings remain inherited without dropped rows.

The [WOW32 minimum-diff admission governance](../etc/operations/wow32-queue-minimal-diff-admission-governance.md)
links every remaining WOW32 candidate to the OpenNT-only seam ledger and
classifies its admission gate. It does not reorder this Queue, admit work, or
turn static audit closure into product/runtime completion. Every candidate
proposal applies that rule before adding a mirror difference or adapter. The
active-packet transition does not change any candidate's proposed scope.

| Remaining stable order | Candidate implementation package | Proposal |
| --- | --- | --- |
| 1 | WOW32 DDE — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-dde-001.md) |
| 2 | WOW32 sound and multimedia — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-sound-multimedia-001.md) |
| 3 | WOW32 Winsock — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-winsock-001.md) |
| 4 | WOW32 printing and spool — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-printing-spool-001.md) |
| 5 | WOW32 COMM — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-comm-001.md) |
| 6 | WOW32 ToolHelp and WOW debugger interfaces — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-toolhelp-debugger-001.md) |
| 7 | WOW32 whole-provider and three-application acceptance — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-integrated-acceptance-001.md) |
| 8 | OpenNT error-dialog response and termination semantics recovery — restore and verify residual source-defined response and completion paths; reuse WOW-owned results, bringing forward only dependencies needed for safe WOW execution. | [Proposal](../proposals/proposal-error-dialog-termination-semantics-restoration-001.md) |
| 9 | MVDM whole-closure completeness audit and recovery — repair missing/cropped/unconnected originals and remove the local providers displaced by those recoveries; hand off the reconciled inventory to minimization. | [Proposal](../proposals/proposal-mvdm-whole-closure-recovery-001.md) |
| 10 | Project-wide original-source implementation minimization review — reuse the closure inventory to consolidate remaining duplicate or unnecessary implementations, mirror diffs, overlays and bindings without losing verified capabilities. | [Proposal](../proposals/proposal-original-source-implementation-minimization-review-001.md) |
| 11 | Windows 3.1 ordinary-Setup post-copy protected-mode transition recovery — restore a valid normal-Setup witness, locate the actual DOSX/first-Windows source boundary after file copy, and repair only a proved project-owned or source-shaped host cause. | [Proposal](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md) |
| 12 | External system launch loader — independently install a 32/64 global launch integration that routes qualifying DOS/Win16 launch requests to the configured `ntvdm64` `run16` runtime, while leaving core runtime ownership and native launches independent. | [Proposal](../proposals/proposal-external-system-launch-loader-001.md) |
| 13 | Win3.1 interactive performance root-cause recovery — repair the failed active-interaction measurement path, attribute actual input/IRQ/CCPU/VGA/presentation cost, then change only a proved project-owned bottleneck while retaining independently correct optimizations. | [Proposal](../proposals/proposal-win31-interactive-performance-root-cause-001.md) |

[Prior planning and unresolved audit-family mapping](../etc/operations/queue-planning-history.md)
are retained as supporting history, not additional queued or admitted tasks.
The [closed broker proposal](../proposals/proposal-cross-process-broker-closure-001.md)
also remains linked for requirement traceability; it is not a queued candidate.

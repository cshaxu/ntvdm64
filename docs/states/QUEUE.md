# Queue

This is the authoritative ordered queue of unnumbered candidate packages.
Only [Current](CURRENT.md) admits an active task. The lifecycle and WOW32
execution-continuation packages are owner-closed; the former first candidate
is now admitted and represented only in Current. Remaining candidates retain their relative order and
residual scope. The [WOW32 program](../proposals/proposal-wow32-production-completion-002.md)
and its approved cross-package mappings remain inherited without dropped rows.

| Next order | Candidate implementation package | Proposal |
| --- | --- | --- |
| 1 | WOW32 modules, memory and aliases — retain general loader/allocation/alias coverage; reached EDIT/dialog DS and startup-failure coordination transfer to the active execution package. | [Proposal](../proposals/proposal-wow32-modules-memory-aliases-001.md) |
| 2 | WOW32 files, directories, environment and OEM — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-files-environment-oem-001.md) |
| 3 | WOW32 resource discovery and loading foundation — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-resource-loading-001.md) |
| 4 | WOW32 GDI identity, DC and drawing — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-gdi-dc-drawing-001.md) |
| 5 | WOW32 fonts, text and metafiles — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-fonts-text-metafiles-001.md) |
| 6 | WOW32 residual dialogs/input/hooks/timers — reuse execution-owned basic controls/dialogs and hook entry; complete broader input/caret/keyboard, hook registration/removal and timers. | [Proposal](../proposals/proposal-wow32-dialogs-input-hooks-timers-001.md) |
| 7 | WOW32 residual resource conversion/accelerators — reuse execution-owned MENU-01 graph/parser/mutation/retirement; retain unrelated icon/cursor/bitmap conversion and display refinements. | [Proposal](../proposals/proposal-wow32-menus-resource-conversion-001.md) |
| 8 | WOW32 common dialogs and related OLE interfaces — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-common-dialogs-ole-001.md) |
| 9 | WOW32 clipboard — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-clipboard-001.md) |
| 10 | WOW32 shell and layered Registry — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-shell-registry-001.md) |
| 11 | Existing WOW window drawing residue — reproduce WINMINE's owner-reported pixels below the menu, identify the actual painter and client/non-client boundary, then repair the proved cause without cosmetic masking or guest changes. | [Proposal](../proposals/proposal-wow-window-drawing-residue-001.md) |
| 12 | WOW32 everyday three-application usability acceptance — verify correct display, interaction, ordinary file operations and cleanup before optional service completion; not whole-provider acceptance. | [Proposal](../proposals/proposal-wow32-everyday-usability-001.md) |
| 13 | WOW32 WOW hard-error responses — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-hard-error-responses-001.md) |
| 14 | WOW32 DDE — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-dde-001.md) |
| 15 | WOW32 sound and multimedia — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-sound-multimedia-001.md) |
| 16 | WOW32 Winsock — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-winsock-001.md) |
| 17 | WOW32 printing and spool — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-printing-spool-001.md) |
| 18 | WOW32 COMM — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-comm-001.md) |
| 19 | WOW32 ToolHelp and WOW debugger interfaces — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-toolhelp-debugger-001.md) |
| 20 | WOW32 whole-provider and three-application acceptance — complete this original-owner capability with guest, failure and cleanup tests; inherit the shared program gates. | [Proposal](../proposals/proposal-wow32-integrated-acceptance-001.md) |
| 21 | OpenNT error-dialog response and termination semantics recovery — restore and verify residual source-defined response and completion paths; reuse WOW-owned results, bringing forward only dependencies needed for safe WOW execution. | [Proposal](../proposals/proposal-error-dialog-termination-semantics-restoration-001.md) |
| 22 | MVDM whole-closure completeness audit and recovery — repair missing/cropped/unconnected originals and remove the local providers displaced by those recoveries; hand off the reconciled inventory to minimization. | [Proposal](../proposals/proposal-mvdm-whole-closure-recovery-001.md) |
| 23 | Project-wide original-source implementation minimization review — reuse the closure inventory to consolidate remaining duplicate or unnecessary implementations, mirror diffs, overlays and bindings without losing verified capabilities. | [Proposal](../proposals/proposal-original-source-implementation-minimization-review-001.md) |
| 24 | Windows 3.1 ordinary-Setup post-copy protected-mode transition recovery — restore a valid normal-Setup witness, locate the actual DOSX/first-Windows source boundary after file copy, and repair only a proved project-owned or source-shaped host cause. | [Proposal](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md) |
| 25 | External system launch loader — independently install a 32/64 global launch integration that routes qualifying DOS/Win16 launch requests to the configured `ntvdm64` `run16` runtime, while leaving core runtime ownership and native launches independent. | [Proposal](../proposals/proposal-external-system-launch-loader-001.md) |
| 26 | Win3.1 interactive performance root-cause recovery — repair the failed active-interaction measurement path, attribute actual input/IRQ/CCPU/VGA/presentation cost, then change only a proved project-owned bottleneck while retaining independently correct optimizations. | [Proposal](../proposals/proposal-win31-interactive-performance-root-cause-001.md) |

[Prior planning and unresolved audit-family mapping](../etc/operations/queue-planning-history.md)
are retained as supporting history, not additional queued or admitted tasks.
The [closed broker proposal](../proposals/proposal-cross-process-broker-closure-001.md)
also remains linked for requirement traceability; it is not a queued candidate.

# Queue

This is the authoritative ordered queue of unnumbered candidate packages.
Only [Current](CURRENT.md) admits an active task. The lifecycle, USER-object,
selected classic-DDE and sound/multimedia packages are owner-closed. Remaining candidates retain their relative scope; the table
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
| 1 | Unified tools/build.cmd build and release — clean up build entrypoints, build all eleven product components and every independently authored add-on in dependency order, and publish the verified complete outputs to assets/release. | [Proposal](../proposals/proposal-unified-build-cmd-release-001.md) |
| 2 | Upgrade to a Windows XP NTVDM baseline — assess and recover XP-era capabilities including audio and long file names, update all guest components, and make required modern-host compatibility repairs. | [Proposal](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md) |
| 3 | Windows 2.03 Setup patch package — establish tools/win203-setup and verify installation, startup, display/input, application interaction, exit and restart. | [Proposal](../proposals/proposal-win203-setup-installation-001.md) |
| 4 | Hardware-virtualized guest CPU acceleration — investigate and implement a modern hardware-virtualization execution backend for 16/32-bit guest instructions, subject to demonstrated feasibility and compatibility. | [Proposal](../proposals/proposal-hardware-virtualized-guest-cpu-001.md) |
| 5 | WOW multi-window performance recovery — attribute and reduce the latency of SOL Options and other Win16 secondary windows while preserving original callback, identity and lifecycle behavior. | [Proposal](../proposals/proposal-wow-multiwindow-performance-recovery-001.md) |
| 6 | Windows 95 Setup installation package — prepare a recoverable media copy and run original Setup through a launchable, verified Win95 installation. | [Proposal](../proposals/proposal-win95-setup-installation-001.md) |
| 7 | Windows 1.01 application launch from the host — classify and launch a selected application through its matching Windows 1.01 guest environment, preserving arguments, environment selection and truthful application lifecycle results. | [Proposal](../proposals/proposal-win101-host-application-launch-001.md) |
| 8 | Guest Windows / host bidirectional launch routing research — investigate redirecting guest double-click launches to the host and routing MS-DOS VM host-command requests back to guest Windows, with explicit execution-location choices. | [Proposal](../proposals/proposal-win31-bidirectional-launch-routing-001.md) |
| 9 | Host-rendered Win32 applications inside guest Windows — investigate host execution/rendering with guest Win3.1 window presentation and input forwarding, without Win32s or original guest modifications. | [Proposal](../proposals/proposal-win31-host-window-presentation-001.md) |
| 10 | Win3.1 interactive performance root-cause recovery — repair the failed active-interaction measurement path, attribute actual input/IRQ/CCPU/VGA/presentation cost, then change only a proved project-owned bottleneck while retaining independently correct optimizations. | [Proposal](../proposals/proposal-win31-interactive-performance-root-cause-001.md) |

The external system launch loader candidate was removed from the queue.
Its proposal remains as historical planning material; it is not pending.

The owner accepts the original WOW32 and VDMREDIR PE runtime imports from
NTVDM and removes their elimination candidate from the queue. Its
[proposal](../proposals/proposal-worker-runtime-import-boundary-001.md) is
retained as historical planning material and is not pending.

The owner split the former multitasking frontend/VxD candidate into the host
frontend foundation followed by the guest driver and NTVDM bridge. The split
retains the remaining candidates' relative order. The host foundation is delivered in its
[host foundation record](../proposals/proposal-host-common-frontend-foundation-001.md). The guest driver work was delivered within [closed multitask task](CURRENT.md#recent-m0-closures), with explicit retained boundaries.

On 2026-10-29 the owner moved further SOL Options and related WOW multi-window
performance optimization out of the active task and directed its candidate to
the end of the queue at that time. This does not waive the active task's correctness or test
obligations, including the separately observed native-return error 1400.

Earlier original-recovery admissions and queue reorderings are retained in
the planning history linked below. The table above is the current
queue order; historical positions do not add or reorder pending work.

The owner confirms the former third candidate, Windows 3.1 ordinary-Setup
post-copy protected-mode transition recovery, is complete and directs its
removal. Its [proposal](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md)
is retained for traceability, not pending work. The owner reordered the remaining
candidates and added the Windows XP NTVDM baseline upgrade; this planning
update does not change the active packet.

[Prior planning and unresolved audit-family mapping](../etc/operations/queue-planning-history.md)
are retained as supporting history, not additional queued or admitted tasks.
The [closed broker proposal](../proposals/proposal-cross-process-broker-closure-001.md)
also remains linked for requirement traceability; it is not a queued candidate.

The guest multitasking bridge task is closed; see [closed multitask task](CURRENT.md#recent-m0-closures). Remaining candidates retain their relative order.

The original-code recovery package is closed and its results are integrated
into main; its dedicated worktree has been deleted. The
[current status and closure links](CURRENT.md) retains the findings and
limitations for traceability. It is not an additional queued task, and its
historical inventory coverage does not create pending work. The candidates above retain their current scope and order.

The mirror-diff audit/minimization candidate is admitted in a dedicated
worktree and is no longer pending here. Its [proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md)
retains the scope; the remaining candidates preserve their relative order.

The former windowed DOS text candidate was handled by [closed multitask task](CURRENT.md#recent-m0-closures); it is not a separate queued task.

The guest multitask I/O completion candidate was handled by [closed multitask task](CURRENT.md#recent-m0-closures). Its explicit unresolved boundaries remain in TODO, not a duplicate queued task.

The restart-to-MS-DOS investigation is [closed with explicit transfer](CURRENT.md#recent-m0-closures)
to the Windows XP baseline upgrade above; it is not a separate queued package.
The implementation-minimization review is now admitted through [Current](CURRENT.md).

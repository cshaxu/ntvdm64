# Project Status

**Active: M0 T447 S1 — WOW32 execution boundary and baseline.**

## Current Work

Owner accepts T446 and directs "验收通过 批准收口t任务 准入下一个t任务".
[T446 closure](../history/m0-t446-carrier-visibility-closure.md) records its
accepted delivery ca0abb7c4. The former queue head is now T447:
[WOW32 messages, callbacks and task execution](../proposals/proposal-wow32-message-task-execution-001.md).
Its W01 ownership and inherited evidence are retained. The
[working plan](../etc/operations/t447-wow-execution-plan.md) records S1--S4;
only S1 is active. No production change is admitted by this baseline stage.

## Active Packet

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T447 S1, Ordinary Mode; one implementer and subsequent separate review pass. |
| Admission And Approval | Owner accepts/closes T446 and admits the next queue package, W01 WOW32 execution. |
| Objective | Establish a finite production caller/test matrix for C01--C03, C09 and execution C10; recheck current WINMINE/SOL/WRITE frontiers and inherited E92 failure. |
| Non-goals | No production repair in S1, guest patches, global re-audit, local SMS transport, second scheduler or assumed heap diagnosis; no silent absorption of other W owners. |
| Reference Baseline | Accepted ca0abb7c4 thirteen-image release; T422 E91/E92/E93 and S3 checklist; T430 G03 callback/task handoff; T446 independent application frontiers. |
| Files And ABI Surface | Existing WOW task/callback production paths read-only; tests and indexed evidence/plan may change. No runtime or wire ABI change. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW program gates; original x86 /MT CCPU40; background-only test/build runs. |
| Verification | Pin binaries/media/configuration; rerun the three app frontiers and applicable existing execution probes; classify real guest, native peer and mock evidence separately, including errors and cleanup. |
| Expected Markers | Exact caller/provider mapping, source/build/wiring/focused/guest/teardown status, first observed failing boundary or explicit unresolved result; no mock-only capability completion. |
| Asset Needs | Existing sealed package, immutable guest images, retained MSVC/Ninja and fixtures; new outputs only below build/M0-T447/S1 and approved isolated stages. |
| Reporting Requirements | Keep stable C/W/E/G IDs; report expected/actual results and confidence independently per app; inherited failures remain open. |
| Stop Conditions | Required production repair or another owner dependency must be scoped in the next implementation brief; unknown external inputs or guest mutation cannot be bypassed. |
| Exit Criteria | Reviewed finite matrix and reproducible baseline evidence, documented next repair boundary, governance/link/diff checks, documentation/test delivery commit and push; no functional-completion claim. |
| Original Owner Request | 验收通过 批准收口t任务 准入下一个t任务 |
| Similar-Issue Sweep | Normal/early/duplicate replies; same/cross-task reentry; sender/receiver/task loss; callback frame/lease release; original init/yield/hung/wait paths. |

Admission is documentation only; no new runtime tests or fixes are claimed.
S1 must reproduce the current application frontiers rather than infer their
cause from an OOM dialog. SOL/WRITE normal initialization is a mandatory
basic-functionality milestone across the existing owners, not satisfied by
matching their prior failure. Proven cross-owner dependencies require explicit
bounded promotion. Full production gates apply to subsequent runtime changes.

## Current Technical Baseline

The coherent13-image T446 S1 package is published to assets/release and
O:/winnt/system32. release-manifest.json records exact identities; T446 S1
publication verifies all tested/copy/deployed hashes and published-byte guard
behavior. Twelve unchanged images retain the T445 S11 baseline and its style
boundary evidence.
S9 P1 is266067264 and P2 is4f8c181487; S10 P1 is e046ee3d7;
owner-accepted S11 P1 is b39ceb7e6, acceptance record cbd0f7832.
The preceding T445 final closure changed documentation only; T446 S1 updates
NTVWM's private-carrier guard without changing the other components.
Guest media and ordinary runtime configuration remain unchanged.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll, ntw16s64.dll, W16Style.exe |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll, ntw16s32.dll |

--win16-style and --win16-style-all retain their propagated policy. WOW32
owns Win16 attachment; native Hooks use their matching-width NTW16S DLL.
S11 changes only NTW16S32/64 and W16Style among the13 images: generic dialog
background/link typography, flat separators and multiline EDIT attachment.
The other ten binaries retain the S10 identities. W16Style adds page28 for
dialog/link coverage; native branding and application-owned painting remain.
Title/menu/standard-control System bitmap type is20px. The demo layout
matches20px proportions. Generated caption prefixes are retired; application
titles remain unchanged. NTCON links RPC core without launch-style state.
See [S11 evidence](../etc/evidence/m0-t445-s11-dialog-coverage.md) for actual
verification and limits. SOL/WRITE retain their known initialization failures,
assigned to the existing basic-WOW queue rather than claimed repaired here.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T446 | Owner-accepted event-driven private Console hiding, completion ordering and coherent release. [Closure](../history/m0-t446-carrier-visibility-closure.md) |
| T445 | Modular Win3.x appearance, propagation and integration,20px demo/caption cleanup, default-button and dialog/control coverage. [Closure](../history/m0-t445-window-style-closure.md) |
| T444 | Owner-accepted indexed palette repair, audit-only capability disposition and measured host VT/API fidelity. [Closure](../history/m0-t444-indexed16-text-closure.md) |
| T441 | Repaired mixed NTVDM/NTVWM release that stalled COMMAND/ver; bounded variable I/O retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Verified graphics/transport/idle/build improvements retained; active root cause deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible Win3.1 path repair and CMD/PIF/GRP ownership. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Win3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

Final owner-directed closure includes S10/S11 after the earlier S9 closure.
Earlier stages closed without manual testing remain explicitly distinguished;
S11 acceptance does not retroactively change their evidence. Native-menu
animation and bounded glyph/layout limitations stay in TODO, not represented
as fixed. The whole T retains registered S7 DIV-331; S10/S11 and final
documentation closure add no mirror/guest diff.
[Archived status](../history/m0-t445-pre-closure-status.md) retains the former
detailed chronology; no historical packet there is active.

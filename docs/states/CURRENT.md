# Project Status

**Active: M0 T446 S1 — private Console visibility guard.**

## Current Work

Owner admits Queue head: "准入任务1 开始执行". T445 stays closed.
The [carrier proposal](../proposals/proposal-ntvwm-hidden-carrier-visibility-guard-001.md)
is now T446; its three proposed implementation/integration/test steps are
one bounded S1 delivery so no unused guard is published separately.

## Active Packet

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T446 S1, Ordinary Mode; single implementer followed by sequential review. |
| Admission And Approval | Owner admits queued carrier visibility guard and requests execution. |
| Objective | Keep the exact private NTVWM Console hidden during residency and at direct completion. |
| Non-goals | No polling, target-name special case, presentation cadence, lifetime, protocol, Hook, guest or original mirror changes; transient exposure before asynchronous delivery is not prevented. |
| Reference Baseline | Clean pushed6fc7e3db5; coherent13-image S11 release; existing WinEvent research and carrier proposal. |
| Files And ABI Surface | NTVWM console-state guard and main integration; owned tests/build selection and evidence only. No shared ABI changes. |
| Applicable Rules | Execution, architecture, coding, documentation and source policy; background-only builds/tests. |
| Verification | Actual guard events, exact-window negative, no-HWND, shutdown race, resource cleanup; real native target/completion/reuse and required Product/Control regressions; coherent13-image publication and hashes. |
| Expected Markers | Carrier show-to-hide, unrelated HWND unchanged, no periodic wait, stop/join before close, unchanged completion/output and only ntvwm.exe changed. |
| Asset Needs | Existing MSVC/Ninja, sealed S11 package and original guest fixtures; private desktop probes. No imported assets. |
| Reporting Requirements | Record exact process/window identity, failures and test scope; never equate asynchronous hide with zero transient visibility. |
| Stop Conditions | Need to alter lifecycle, observe non-carrier windows, change presentation pump cadence, or modify another production component requires renewed scope. |
| Exit Criteria | Reviewed implementation, affected/full gates, coherent publication to assets/release and O:/winnt/system32, hash verification, commit/push and clean tree; await owner acceptance. |
| Original Owner Request | Admit queue item1 and begin execution of the NTVWM hidden-carrier guard. |
| Similar-Issue Sweep | Management and fatal close, normal cleanup, startup failure, direct completion KEEP/RELEASE, GUI-only reuse, missing HWND, unrelated windows and show/stop races. |

S1 P1 implementation, AMD64 Release build, focused positive/negative/error
tests, all3 Product and11 Control groups, MySMB Esc and WINMINE gameplay in
Off/Win16/All modes pass; see [evidence](../etc/evidence/m0-t446-s1-carrier-visibility.md).
The coherent13-image set is published and hash-verified in assets/release
and O:/winnt/system32; only ntvwm.exe changes. Published-byte product reuse
verification also passes. This P1 records the reviewed delivery; owner manual
acceptance remains pending. No next task is admitted.

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

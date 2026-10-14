# Project Status

**Active: M0 T445 S10 — default-button repair delivered; awaiting owner acceptance.**

## Current Work

The owner explicitly reopens T445 and admits S10 after reporting the original
Win3.1 About Control Panel OK button as a default-button comparison. The prior
[S1-S9 closure](../history/m0-t445-window-style-closure.md) remains historical
evidence, not acceptance of this newly reported discrepancy. Queue order and
the owner-deferred native-menu animation TODO remain unchanged.

## Active Packet

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T445 S10, Ordinary Mode; one implementer followed by a separate sequential reviewer pass. |
| Admission And Approval | Owner: "重开T445，准入新S修复default button". Reopen only the bounded button correction. |
| Objective | Source-backed Win3.1 default-button appearance and native default/focus/pressed behavior, verified in both style DLL widths and integrated callers. |
| Non-goals | No guest/media changes, new style policy, general UI redesign, mirror change, or reopening of deferred native-menu animation work. Keep20px System type and unchanged captions. |
| Reference Baseline | Clean published/pushed4f8c181487cc69e27f0ca7cb1c1c17ee5872ffa0; S9 r045 coherent13-image publication and linked S9 evidence. Owner screenshot8830ca0d is a comparison, not proof of the underlying control style. |
| Files And ABI Surface | src/ntw16s-dll button painting/state observation, tests/app and tests/observation; W16Style demo only if needed for explicit state coverage. No planned public ABI change. |
| Applicable Rules | Execution, Architecture, Coding, Document and source policy; existing USER stopping boundary and admitted read-only Win3.1 visual research. |
| Verification | Inspect pinned original VGA/USER and dialog resources; compare default versus ordinary, focus, press, disable and default transitions; dual-width positive/negative/lifecycle and visual probes; retained UI/integrated and every-P Product/Control gates; documentation/link/diff review. All builds/tests hidden background under build/M0-T445/S10. |
| Expected Markers | Original resource/style and drawing attribution; independently asserted pixels/state transitions and Enter behavior; no caption/menu/scroll regressions; tested/assets/system32 hash equality for13 images. |
| Asset Needs | Read-only owner Win3.1 installation and retained original extraction evidence; existing MSVC/Ninja and probes. No new imported executable code or font. |
| Reporting Requirements | Explain root cause, default/focus distinction, original owner and minimal correction; disclose failed/unavailable observations. No completion claim before delivery. |
| Stop Conditions | Unproven original behavior, need for guest/mirror changes or new ABI/policy requires evidence and re-admission; unavailable verification remains pending, never passed. |
| Exit Criteria | Reviewed correction, affected builds/tests and mandatory regressions pass; coherent backup/publication to assets/release and O:/winnt/system32, hashes verified, governance pass, commit/push and clean tree; then await owner acceptance. |
| Original Owner Request | Check whether original About Control Panel OK is a default button; compare original VGA implementation and fix our default-button interpretation. Explicitly reopen T445 with a new S. |
| Similar-Issue Sweep | Ordinary/default push buttons, focused/unfocused, pressed/disabled, BM_SETSTYLE/DM_SETDEFID transitions, dialog keyboard activation, native32/64 and WOW callers. |

The original resource confirms OK is a default button. Retail USER comparison
and the owned painter correction are recorded in [S10 evidence](../etc/evidence/m0-t445-s10-default-button.md).
Dual-width focused positive/old-release negative tests, full UI and integrated
runtime gates pass. S10 r015 publishes and verifies the coherent13-image set
at both release locations; this commit delivers S10 P1. Await owner acceptance;
no next S or T is admitted automatically.

## Current Technical Baseline

The coherent13-image T445 package is published to assets/release and
O:/winnt/system32. release-manifest.json records exact identities; S10 r015
verifies tested/copy/deployed hashes and the deployed style boundary.
S9 P1 is266067264 and P2 is4f8c181487; this commit delivers S10 P1.
Guest media and ordinary runtime configuration remain unchanged.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll, ntw16s64.dll, W16Style.exe |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll, ntw16s32.dll |

--win16-style and --win16-style-all retain their propagated policy. WOW32
owns Win16 attachment; native Hooks use their matching-width NTW16S DLL.
Title/menu/standard-control System bitmap type is20px. The demo layout
matches20px proportions. Generated caption prefixes are retired; application
titles remain unchanged. NTCON links RPC core without launch-style state.
See [S9 evidence](../etc/evidence/m0-t445-s9-integrated-acceptance.md) for actual
verification and limits. SOL/WRITE retain their known initialization failures.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T445 | Modular Win3.x appearance, propagated policy, WOW32/native integration and final20px demo/caption cleanup. [Closure](../history/m0-t445-window-style-closure.md) |
| T444 | Owner-accepted indexed palette repair, audit-only capability disposition and measured host VT/API fidelity. [Closure](../history/m0-t444-indexed16-text-closure.md) |
| T441 | Repaired mixed NTVDM/NTVWM release that stalled COMMAND/ver; bounded variable I/O retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Verified graphics/transport/idle/build improvements retained; active root cause deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible Win3.1 path repair and CMD/PIF/GRP ownership. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Win3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

The prior owner-directed T445 closure is superseded only by the explicit S10
reopening above; S1-S9 delivery and acceptance limitations remain preserved.
Earlier stages closed without manual testing remain explicitly distinguished.
Native-menu animation and bounded glyph/layout limitations stay in TODO,
not represented as fixed. [Archived status](../history/m0-t445-pre-closure-status.md)
retains the former detailed chronology; no historical packet there is active.

# Project Status

## Current Work

**Active: M0 T436 S2** (Ordinary Mode; Windows 3.1 mouse-driver implementation).
Owner now authorizes targeted diagnosis of the final-text publication failure
on Windows return (“你调查一下，给我一个确定性的答案”). Test-only API
observation confirms the hidden viewport enlargement failure. Owner now approves
the bounded NTCON storage/viewport repair (“好 批准实现修复”); preserve complete
logical frames, restrict native viewport preparation, and reuse one local helper.
Build/test evidence goes under build/M0-T436/S2/r031-viewport-repair.
Driver50-case mock, real
DPMI16-case checks and actual /S /3 movement/menu observations are recorded in
[S2 evidence](../etc/evidence/m0-t436-s2-win31-mouse-driver.md). The repaired host
passes actual /S return (exit0) and17 Console plus17 Window routes. No S2 closure
or S3 admission is presumed; all
candidate source/evidence is preserved and experimental INI restored.
Owner adds release delivery of both compiled mouse drivers and a Win1.01
interactive media-PATCH preparation entrypoint using those release artifacts.
Implement under build/M0-T436/S2/r037-addon-release with all task writes on O:;
C: tooling is read-only and E:/X:/Y: are excluded. S3 retains actual Win3.1
installer preparation with the same package/installed PATCH independence.
Owner also authorizes committing the obsolete src/interface/README.md deletion.
Owner moves installer tooling to tools/win101-setup and future tools/win31-setup;
driver sources remain src/addon. Preserve the original/installed PATCH workflow.
Both NE driver assets and the separate add-on manifest are synchronized;
Win101 is verified, Win31 remains candidate. Moved-tool package/profile/negative
tests pass; refreshed Win101 ZIP preserves original media. New provider replays
fail before guest test execution at environment setup and remain non-passes.
S2 is still active; no S3 admission or closure is claimed by this delivery.
Owner closes T435 and directly admits the former queue-tail Windows 3.x
candidate, prioritizing Windows 3.1. [T435 closure](../history/m0-t435-win101-addon-closure.md)
records accepted installation/add-on delivery and preserved compatibility limits.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T436 S2; Ordinary Mode; Windows 3.1 mouse ABI audit and independent driver implementation. |
| Candidate Proposal | [Windows 3.x non-WOW feasibility](../proposals/proposal-windows-3x-dos-runtime-001.md). |
| Admission And Approval | Owner: “接受不稳定运行；准入S2鼠标驱动，和之后的S3安装程序补丁。” S1 concludes as explicitly accepted limited research; admit S2 now, S3 sequentially afterwards. |
| Objective | Audit and implement an independently authored Windows 3.1 mouse driver using the existing NTVDM mouse provider; verify standard/enhanced entry, movement, clicks, show/hide and cleanup. Preserve original guest/host execution and default profiles. |
| Non-goals | No CPU/device/mirror rewrite, runtime hot-patch, new helper/component/protocol or renewed enhanced-startup investigation. Do not blindly reuse the Win1.01 real-mode callback in protected Win3.1. Installer delivery belongs to S3; enhanced instability remains an explicit accepted limitation. |
| Reference Baseline | Published T435 S2 APP0.0.435/RPC45/I/O25 ten images and matching assets/release manifest; accepted Windows 1.01 guest execution/mouse and S3 installation workflow. |
| Files And ABI Surface | src/addon/win31-mouse-drv independent guest driver; approved NTCON repair; assets/release/MOUSE101.DRV and MOUSE31.DRV with separate add-on manifest; tools/win101-setup interactive application script and tests. Build under build/M0-T436/S2. Driver/profile selection only on recoverable copies or installation PATCH; originals and defaults remain intact. No mirror/guest/wire change. |
| Applicable Rules | AGENTS reading set, EXECUTION, architecture/coding/document rules, source policy and original guest immutability; inherited host defects only matching existing SoftPC correction, project defects separately designed. |
| Verification | Exact Win3.1 NE/export/Inquire/event/callback ABI; real/protected-mode transition and callback ownership; guest-build positives/negatives and real INT33 route. Observe actual arrow movement and down/up behavior, balanced enable/disable and restart in /S and /3, distinguishing inherited startup failures. Pin owned processes, preserve hashes, no mappings. Governance/link/diff review. |
| Expected Markers | Built independent driver, ABI/relocation proof, reversible provider callback installation, actual Windows input consumption, mode-specific pass/failure evidence and no original/default changes. |
| Asset Needs | Discover owner-supplied Windows 3.1 installation/media read-only, including O:/win31 if still available; no download/import or missing-media success assumption. Any comparison environment remains evidence only, never product dependency. |
| Reporting Requirements | Version/mode, inputs, original-source provenance, current capabilities, gap attribution, feasibility confidence and decisions requiring owner approval. |
| Stop Conditions | Missing/unproven media or provenance, requested guest mutation, architecture expansion, destructive original changes or unsupported claim: preserve and report; do not guess or silently implement. |
| Exit Criteria | Driver source/build/ABI and provider tests pass; actual standard/enhanced mouse interaction and cleanup are proved or exact inherited mode limitations are retained under owner acceptance. Review/test/commit/push before S3 admission. No startup failure counted as driver pass. |
| Original Owner Request | “接受不稳定运行；准入S2鼠标驱动，和之后的S3安装程序补丁。” Updated goal: “建立类似win101的鼠标驱动和安装包补丁”。 |
| Similar-Issue Sweep | Relative/absolute event flags, button count, real/protected callback addresses and stacks, DPMI transition, existing INT33 callback restoration, competing INT71 consumer, mode changes and Windows ownership. |

## S1 Closure Record

Owner accepts unstable enhanced execution on2026-10-07. The
[S1 evidence](../etc/evidence/m0-t436-s1-win31-launch-profile.md) records the
checked retail-copy substitution, actual /3 desktop/Notepad, independent
startup failures and preserved original/default hashes. Standard-profile,
adaptation positive/negative, ten-image identity and governance checks pass.
Research/design delivery42fd2c883 is committed and pushed to main. No host
runtime change or stable-startup/normal-exit/mouse claim. S2 is admitted;
its first guest driver build is not published or runtime-verified.

## Current Technical Baseline

T436 S2 r031 publishes the bounded NTCON viewport repair at O:/winnt/system32,
APP0.0.435/RPC45/I/O25 unchanged. Its [S2 evidence](../etc/evidence/m0-t436-s2-win31-mouse-driver.md)
records actual /S return and focused/product verification. Nine unchanged images
match T435 S2 r060; NTCON hash starts3927A308. assets/release/release-manifest.json
matches all ten published images. The containing reviewed repair delivery records
34 product passes and three retained WOW frontiers; the remaining mouse-driver
candidate stays active and S3 is not admitted.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll |

CCPU40 remains the guest machine; CPU30, guest core mutation and substitute
execution backends are not admitted. Future product-code commits include the
latest verified complete ten-image assets under EXECUTION's new gate.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T435 | Owner-accepted Windows 1.01 INT33 add-on, original Setup/profile delivery and assets. [Closure](../history/m0-t435-win101-addon-closure.md). |
| T434 | Owner-aligned live Direct/Observed monitoring. [Closure](../history/m0-t434-direct-observed-monitor-closure.md). |
| T433 | Native components and mixed-width/PIF recovery. [Closure](../history/m0-t433-native-components-x64-closure.md). |

T435 S3 deliverydd7cddd7d and asset follow-up9e2b16170 are pushed. Direct
Windows 1.01 Notepad classification remains the recorded original-rule limit,
not proof of WOW execution. Physical/RDP/general-app limits remain explicit.

## Accepted Predecessor And Next Admission

Owner fixes the [T436 stage sequence](../etc/operations/t436-win31-setup-mouse-plan.md):
S1 accepted limited enhanced startup research; S2 active mouse driver; S3 installer/
patch package with winstd.cmd/pif and win386.cmd/pif. Admit sequentially after
each predecessor concludes. S3 is not active; all three deliveries precede
the integrated owner acceptance, and T436 stays open until that verdict.

S1 concludes under the owner's explicit unstable-execution acceptance. S2 is
active; S3 follows its reviewed driver delivery. The following retained
startup observations are evidence, not a stable-enhanced compatibility claim.
All other unadmitted Queue candidates retain relative order.
Owner-directed enhanced trials now confirm DOSMGR's ten-CON SFT probe fails:
r023 records10 successful opens, failure message and batch continuation;
r022 control records0 opens and DOS5.00.500. Normal configuration controls do
not fix it. [S1 evidence](../etc/evidence/m0-t436-s1-win31-launch-profile.md)
records exact binaries, disassembly, tests, restored configs and limitations.
Owner now explicitly approves that retail-copy DOSMGR experiment. The resumed
goal keeps full enhanced-mode startup/diagnosis scope; no success is presumed.
The recoverable E386 copy has a checked SFT adaptation. r036/r039/
r043 capture actual forced-/3 desktop; r043 also opens guest Notepad. Independent
replays still fail, including r044 with original PIF idle detection disabled.
This remains an unstable research candidate, not closed/verified delivery.
The later r051 fallback preserves actual CON opens/close/free; r059/r060
published-package replays still time out. Host heartbeat/PIT state advances,
so a completely stopped timer is not established. The separate diagnostic
RT package's161 is unlocalized and its path-length hypothesis is rejected.
No production host/mirror fix or S2 admission is claimed from these probes.
r062 reselects the smaller pre-allocation discovery substitution and reaches
Program Manager; r063 regenerates its exact bytes, with r065 contract/negative
tests passing. Independent r064/r066 still time out. Quiesced diagnostic
snapshots show actual VMM/V86 transitions, not a proved A20/CPU defect.
The current E386 candidate is the minimal ten-byte substitution again;
owner disposition of unresolved startup reliability is requested, not assumed.
Bounded trials and safe source comparisons are complete for this checkpoint.
Owner accepts unstable execution and directs S1's limited conclusion and S2
admission. The unresolved startup issue is retained, not reported fixed.
S1 changes/evidence still require reviewed commit/push; S2 admission does not
pretend that delivery has happened. Stop further startup-only investigation.

## Recent Governance

Owner-required ten-image assets synchronization is recorded in EXECUTION;
9e2b16170 delivers matching published/tested assets. This admission is
documentation-only and does not require a new runtime build or publication.

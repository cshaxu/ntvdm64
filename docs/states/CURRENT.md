# Project Status

## Current Work

**Active: M0 T436 S3** (Ordinary Mode; Windows 3.1 setup/PATCH package).

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T436 S3; Ordinary Mode; Windows 3.1 original-Setup PATCH packaging. |
| Candidate Proposal | [Windows 3.x non-WOW feasibility](../proposals/proposal-windows-3x-dos-runtime-001.md); [T436 stage sequence](../etc/operations/t436-win31-setup-mouse-plan.md). |
| Admission And Approval | Owner approves the S1 → S2 → S3 sequence, directs the Win3.1 installation patch as S3, and requires the explicit `WINSTD.CMD/.PIF` and `WIN386.CMD/.PIF` interfaces. S2 is concluded in [its evidence](../etc/evidence/m0-t436-s2-win31-mouse-driver.md). |
| Objective | Deliver `tools/win31-setup`, which prepares unchanged original Win3.1 media plus a self-contained PATCH, invokes original Setup, and postconfigures an installed PATCH with MOUSE31, mode-specific PIFs/launchers and isolated profiles. |
| Non-goals | No host product/protocol change, guest-image rewrite beyond approved checked S1 retail adaptation on a recoverable copy, default NT profile edit, helper, drive substitution, hard-coded media/install/run16 path, or claim that unstable enhanced startup is reliable. |
| Reference Baseline | T436 S2 release `MOUSE31.DRV` hash `D6BA5380A2EDCDB33E0C36850FB6BDD0542D78EC03E982DBD5114A462B0A2F1A`; current synchronized ten-image package; migrated `tools/win31-setup` is the S3 implementation input, not yet the final package. |
| Files And ABI Surface | `tools/win31-setup` scripts/templates/readme and integration tests; displaced `src/addon/win31-launch` is rehomed and callers swept. Generated media and installed additions stay below owned `PATCH`; use `assets/release/MOUSE31.DRV`. |
| Applicable Rules | AGENTS reading set, EXECUTION, architecture/coding/document rules, source policy, guest immutability, release-asset rule, O:/Z: write boundary and no drive substitution. |
| Verification | Fresh build-owned media/package fixtures; PIF extension/checksum and path-field verification; source/add-on hashes; standard and enhanced launcher/profile shape; original/default-media non-mutation; setup/package independence; normal/negative/relocation checks. Real /S and /3 observations are recorded separately and do not turn accepted enhanced instability into a pass. |
| Expected Markers | Media `PATCH` holds authored payload only; installed `PATCH` has exactly `WINSTD.CMD/.PIF`, `WIN386.CMD/.PIF`, required profiles and MOUSE31; no file refers to package WORK/repository; `run16` resolves from PATH. |
| Asset Needs | Owner-supplied original Win3.1 media at an explicit path; released MOUSE31 and its add-on manifest; selected approved S1 adaptation only with exact before/range/hash checks. |
| Reporting Requirements | Record inputs/hashes, created files, original Setup exit separately from confirmed installation, mode-specific runtime outcomes and all unsupported/unstable boundaries. |
| Stop Conditions | Missing/provenance-invalid media, a required guest mutation outside approved adaptation, a path that cannot fit PIF fields, unknown preexisting PATCH overwrite, or an attempted production-protocol/host-component expansion. |
| Exit Criteria | Fresh package/install profile tests cover all four requested entrypoints, immutable/default media remains unchanged, no installed dependency on setup package, source/asset hashes and PIF chains pass, required runtime observations are honest, then review/commit/push for owner hand-test. |
| Original Owner Request | “建立类似win101的鼠标驱动和安装包补丁”; “Winstd.cmd, winstd.pif, win386.cmd, win386.pif”; shared `AUTOEXEC.NT`/`CONFIG.NT` only when genuinely compatible. |
| Similar-Issue Sweep | PIF main/386/NT extension arguments, checksums, field capacity, Setup return/confirmation, SETVER/DOSX policy, copied WORK/profile references, relocation, PATH-based run16, original versus installed PATCH, standard versus enhanced semantics. |

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

T436 S2 publishes the bounded NTCON viewport repair at O:/winnt/system32,
APP0.0.435/RPC45/I/O25 unchanged. Its [S2 evidence](../etc/evidence/m0-t436-s2-win31-mouse-driver.md)
records actual /S return, real mouse consumption and focused/product verification.
All ten published hashes match `assets/release`; NTCON is
`9E480B890DF70EF346FA8927F93650CB96D0165E6CD0DBCA852E0EB34761B82A`.

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
S1 accepted limited enhanced startup research; S2 mouse driver; S3 installer/
patch package with winstd.cmd/pif and win386.cmd/pif. Admit sequentially after
each predecessor concludes. S3 is active; all three deliveries precede
the integrated owner acceptance, and T436 stays open until that verdict.

S1 concludes under the owner's explicit unstable-execution acceptance. S2 is
now concluded; S3 is separately admitted above. The following retained
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

## S2 Closure Record

S2 is concluded in [the Win3.1 mouse-driver evidence](../etc/evidence/m0-t436-s2-win31-mouse-driver.md):
the released `MOUSE31.DRV` matches its reproducible build, standard-mode real
mouse/exit proof passes, and the separately accepted `/3` instability remains
explicitly limited. S3 may therefore package the released driver but may not
claim that enhanced instability is repaired.
S1 changes/evidence still require reviewed commit/push; S2 admission does not
pretend that delivery has happened. Stop further startup-only investigation.

## Recent Governance

Owner-required ten-image assets synchronization is recorded in EXECUTION;
9e2b16170 delivers matching published/tested assets. This admission is
documentation-only and does not require a new runtime build or publication.

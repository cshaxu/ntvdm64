# Project Status

## Current Work

**Active: M0 T435 S3** (Ordinary Mode; original Setup installation and launch profile).
Owner admits Windows 1.01 support and confirms EGA is tested/passed; mouse
repair is pending. [T434 closure](../history/m0-t434-direct-observed-monitor-closure.md)
remains accepted. The selected Windows 1.01 candidate is removed from Queue;
no production mouse patch, guest change or deployment is claimed by admission.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T435 S3; Ordinary Mode; original Setup installation and Win31-shaped launch profile. |
| Candidate Proposal | [Windows 1.01 EGA/InPort](../proposals/proposal-windows-101-ega-inport-runtime-001.md). |
| Admission And Approval | Owner explicitly requests original Setup installation at O:/win101 after publication, plus start.cmd and matching PIF based on O:/win31. S2 P263118b33/7d273e11c is delivered; this request admits S3 and the named external destination. |
| Objective | Deliver a complete, repo-independent installer directory for owner-run original Setup. Generate Setup and installed PIF/config/autoexec plus win.cmd only; invoke run16 through PATH, never a fixed location. Owner chooses the installation directory interactively. |
| Non-goals | No product/mirror/guest-core algorithm change, fake setup completion, copied prebuilt WIN100 image presented as Setup output, new host component/channel or host Windows mutation. O:/win31 and O:/Windows remain read-only. |
| Reference Baseline | Published S2 APP435/RPC45/I/O25 ten images; authored r008 driver and r062 real same-worker mouse/exit proof. O:/win31 START31.CMD, WIN31.PIF and PROFILE files are read-only launch references. |
| Files And ABI Surface | Owner-renamed src/addon/win101-mouse-drv holds the driver; src/addon/win101-setup owns installer/patch scripts and templates. Test-only guards/probes stay in tests. Packaging at O:/win101-setup keeps original media at root and all additions/work copies under PATCH. Staging/backups under build/M0-T435/S3. Installed additions also use O:/win101/PATCH. Existing PIF declarations, no new protocol. |
| Applicable Rules | AGENTS authorities, source policy/original guest immutability, matching SoftPC fix only for inherited host defects, mirror minimality/register, added adaptation fixes at NTVDM owner. |
| Verification | Original Setup completion, produced BIN/OVL mouse-module identity and real PIF/config/launch/exit checks. Owner forbids drive mappings/hardcoded paths: package derives from the script; Setup remains interactive and prompts for the installed destination afterwards. Packaging inputs are explicit parameters. Temporary WORK is removed on completion. Use actual short paths; mapped-drive runs are historical only. Preserve original/reference and published-package hashes. |
| Expected Markers | Setup actual success and generated WIN.COM/WIN100.BIN/WIN100.OVL; authored MOUSE module; win.cmd runs WIN101.PIF with Win1.01-correct arguments and profile; real color desktop/mouse/menu/normal exit. |
| Asset Needs | Owner now selects pristine H:/ flat original media as the packaging source, replacing the earlier O:/Windows merged installation input. H:/, O:/Windows and O:/win31 are read-only. r008 MOUSE101.DRV; published S2 package; accepted Win31 PIF reference. |
| Reporting Requirements | Exact original/generated hashes, Setup page/write evidence, profile differences, actual launch/return results, recovery and unresolved boundaries. |
| Stop Conditions | Mutation outside build-owned staging, explicitly requested O:/win101-setup package and O:/win101 destination; unrelated media overwrite, unverified target, unrecoverable existing user installation or published product-package changes. |
| Exit Criteria | Actual Setup installation, launcher/PIF/profile and real startup/mouse/exit proof; otherwise preserve partial installation and report unmet requirement, never substitute S2 binary patch for Setup. |
| Original Owner Request | “发布完，用 Setup 安装到 O:/win101 ，参照 O:/win31 ，生成 start.cmd 与对应 PIF”. |
| Similar-Issue Sweep | Target path and CWD, Win31-only /S removal, SETVER for Win1.01, DOS-only guest installer child execution, temp/profile paths, checksum/extension-chain preservation, original installation and published-package integrity. |

## S1 Closure Record

Bounded source/ABI design concluded; [S1 evidence](../etc/evidence/m0-t435-s1-win101-mouse-audit.md)
records original INT71 ownership, actual Win1.01 absolute event consumer and
selected independent INT33 add-on route. Readonly contract audit and
documentation governance pass. No new guest runtime capability claimed.
P delivered as7466a69b1 and pushed. Design delivery does not waive S2 runtime
gates or claim a functioning new mouse driver.

## Current Technical Baseline

T435 S2 r060 ten-image package is published at O:/winnt/system32;
APP0.0.435/RPC45/I/O25. r065 publication and r068 deployed smoke match its
tested hashes. Prior T434 r037 is preserved, with recovery copies under r065.
Original CCPU40 and guest/mirror contracts remain; O:/Windows is unchanged.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll |

[S5 evidence](../etc/evidence/m0-t434-s5-monitor-fidelity.md) retains full14,
Console17/Window17/WOW, native12/DOS7, mixed22/typeahead6, service29, faults and
publication/smoke/hashes. No universal observation/WOW/gameplay claim.

## Reusable Build Inputs

Validated T434 native caches: S3/r002-service, r003-hook64, r004-worker,
r005-frontend, r006-launcher; final monitor S5/r024-monitor. Formal x86/NTVDM
S3/r001-formal and WOW32 S3/r009-wow32. Sealed r037 remains immutable.
Any repair rebuilds its dependency-selected closure, not these evidence packages.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T434 | Owner-aligned readonly Direct/Observed monitoring. [Closure](../history/m0-t434-direct-observed-monitor-closure.md). |
| T433 | Native migration and mixed-width/PIF recovery. [Closure](../history/m0-t433-native-components-x64-closure.md). |
| T432 | Single worker/dual Hooks. [Closure](../history/m0-t432-single-worker-dual-hook-closure.md). |

## S2 Closure Record

[S2 evidence](../etc/evidence/m0-t435-s2-int33-driver.md) records source/ABI,
real Windows mouse/repeated exit, final-package guest checks, all14 gates and
publication/smoke. P263118b33 and status stamp7d273e11c are pushed. Physical
Raw Input/RDP and broad Win1.01 application compatibility are not claimed.

## Next Admission

S2 implementation, runtime and publication boundary is complete: r062 real
Windows movement/menu down/up, two starts/exits on the same worker and actual
MEM continuation; r066/r067 final-package INT33/mock47 checks; r056 installation
positive/four refusals; r063 product3 plus r064 control11 with identical ten
hashes; r065 publication/r068 deployed smoke. [S2 evidence](../etc/evidence/m0-t435-s2-int33-driver.md)
retains original defect, failed attempts, source/ABI and physical input limits.
S2 P delivered as263118b33 and pushed. T435 stays open for owner acceptance.
Owner admits S3 installation/profile work above, then takes over interactive
Setup manually. The private Setup was quit normally without installation;
the owner's later run produced WIN.COM/BIN/OVL at O:/win101, but its Setup
exit code is unknown. r013 generates missing installed PATCH launch profiles,
preserving all existing root file hashes. The owner relocates the package;
the currently inspected copy is O:/w1setup/PATCH/SETUP.CMD. All96 original H:/ root files match exactly,
including original MOUSE.DRV. New driver/source/scripts/config/SETVER and their
working copies are only under PATCH. H: is unmounted; its retained96 SHA-verified
bytes are recovered under r010-frozen-h. r012-patch-package identity/layout/PIF
checks pass; prior directory is recoverable
under its previous-package. [S3 evidence](../etc/evidence/m0-t435-s3-setup-package.md)
records failures and remaining real-installation checks. Installation scripts
now start without arguments and ask for the installed destination after original
Setup returns; packaging input paths remain explicit. No drive mapping or fixed
machine path remains. r018 profile/orchestration fixtures pass, including WORK
cleanup on success/failure, without original Setup or guest execution. r019
updates the actual package scripts. Owner reports installation success and
requests win.cmd only. r020 removes installed alias/package template recoverably;
r021 generation/cleanup tests pass. Owner now confirms final installation and
Windows-internal Notepad work. r034 final fixtures cover copied profile repair,
nonzero-return confirmed recovery and cleanup; SETUP.CMD pauses and preserves
exit status. Direct Notepad is rejected by the original NE classification rule
(r035), not a proven WOW load failure. Owner asks to commit all assets, including
their ten-image snapshot; it is not substituted for the verified S2 runtime.
Real installed physical mouse/exit checks remain unclaimed. This P includes
S3 installer/source rename/docs/assets; T435 stays open, no next task admitted.

## Recent Governance

T435 S1 reaches its source/design conclusion, not runtime completion; see
[S1 evidence](../etc/evidence/m0-t435-s1-win101-mouse-audit.md). Owner's active
goal admits sequential S2 implementation. EGA remains accepted; S2 admission
does not change the installed package or claim working new-driver interaction.

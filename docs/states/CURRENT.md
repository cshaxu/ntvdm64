# Project Status

## Current Work

**Active: M0 T435 S2** (Ordinary Mode; delivery/review of independent Win1.01 INT33 guest driver).
Owner admits Windows 1.01 support and confirms EGA is tested/passed; mouse
repair is pending. [T434 closure](../history/m0-t434-direct-observed-monitor-closure.md)
remains accepted. The selected Windows 1.01 candidate is removed from Queue;
no production mouse patch, guest change or deployment is claimed by admission.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T435 S2; Ordinary Mode; independent guest-driver implementation/build/validation. |
| Candidate Proposal | [Windows 1.01 EGA/InPort](../proposals/proposal-windows-101-ega-inport-runtime-001.md). |
| Admission And Approval | Owner's active goal authorizes research/design/implementation of the independent Win1.01 INT33 bridge and explicitly places it in src/ADDON/Mouse Driver 101. S1 concludes its bounded source/ABI design in the linked evidence; guest implementation proceeds sequentially, no runtime capability claimed. |
| Objective | Build an independent NE mouse driver using unchanged INT33 provider and actual Win1.01 absolute event ABI; verify callback/state/rollback mechanics and real Windows movement/click/release/normal return. |
| Non-goals | No EGA redesign, NTVDM mirror change, fake hardware detection, Ignore workaround, CPU30, helper, new host component/channel, NTSRV/NTCON policy or publication-clock change. Owner now explicitly allows binary installation of the independent mouse module in a recoverable Win1.01 image copy; original media/files remain intact, no runtime hot patch or unrelated core algorithm rewrite. |
| Reference Baseline | T4349cf24314c/r037 APP434/RPC45/I/O25 installed ten images; original CCPU40. Owner EGA acceptance and original Mouse6.24/WIN100.BIN installation retained; old RPC42 diagnostic is evidence only, not current runtime proof. |
| Files And ABI Surface | src/ADDON/Mouse Driver 101 guest ASM/README; tests/observation build, contract and guest harness scripts. Normal-exit evidence admits the minimal project-added console_text / mvdm_softpc_text_video palette repair: call the existing original VGA resolver before final text publication when its copied palette is absent. No host mirror/transport change. NASM isolated16-bit NE/COM island, no guest object in host graph. |
| Applicable Rules | AGENTS authorities, source policy/original guest immutability, matching SoftPC fix only for inherited host defects, mirror minimality/register, added adaptation fixes at NTVDM owner. |
| Verification | Reproduce S1 readonly audit; NASM build and NE validation; executable guest mock/actual INT33 probes; owner-approved localized mouse-module installation in a recoverable build-owned image copy, real Win1.01 callbacks/menu/release/exit and same-worker repeat; affected DOS/native/WOW gates before production P. Fresh outputs below build/M0-T435/S2; only Z: short-path mapping. |
| Expected Markers | Correct three Windows entrypoints; normalized absolute movement with bit15, Win1.01 button bits1..4 and DX2; state/rollback/Disable, no extra DOS pointer; real Windows acceptance distinguished from fixtures. |
| Asset Needs | O:/Windows original installed files read-only source for separate build-owned test copy; current T434 package and existing observers; NASM. New driver is authored guest code, not patched original media. |
| Reporting Requirements | Exact build/hash/provenance, assertions and real guest milestones; distinguish mock, original captured consumer and current runtime; retain independent Invalid handle error and unrepaired InPort defects. |
| Stop Conditions | Original guest/core/media mutation, no bounded Windows driver ABI, new host process/channel/provider policy, irreversible user-install overwrite or hidden runtime failure. |
| Exit Criteria | Built driver with reproducible tests; real Windows mouse/exit proof and retained acceptance gates; otherwise report missing boundaries, no capability closure. |
| Original Owner Request | “调研和设计和实现适配于Windows 1.01的NTVDM的鼠标驱动”; “src 底下加个目录 ADDON…Mouse Driver 101”. |
| Similar-Issue Sweep | Absolute endpoints/geometry, simultaneous buttons, repeated enable/disable, missing provider, size validation, callback RETF/stack, DOS restoration and repeat launch. |

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

## Next Admission

S2 implementation, runtime and publication boundary is complete: r062 real
Windows movement/menu down/up, two starts/exits on the same worker and actual
MEM continuation; r066/r067 final-package INT33/mock47 checks; r056 installation
positive/four refusals; r063 product3 plus r064 control11 with identical ten
hashes; r065 publication/r068 deployed smoke. [S2 evidence](../etc/evidence/m0-t435-s2-int33-driver.md)
retains original defect, failed attempts, source/ABI and physical input limits.
Git P formation is the remaining delivery step; T435 stays open for owner
acceptance. No next S/T is admitted and Queue ordering is unchanged.

## Recent Governance

T435 S1 reaches its source/design conclusion, not runtime completion; see
[S1 evidence](../etc/evidence/m0-t435-s1-win101-mouse-audit.md). Owner's active
goal admits sequential S2 implementation. EGA remains accepted; S2 admission
does not change the installed package or claim working new-driver interaction.

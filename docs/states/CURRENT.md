# Project Status

## Current Work

**Active: M0 T435 S1** (Ordinary Mode; Windows 1.01 mouse source audit/design).
Owner admits Windows 1.01 support and confirms EGA is tested/passed; mouse
repair is pending. [T434 closure](../history/m0-t434-direct-observed-monitor-closure.md)
remains accepted. The selected Windows 1.01 candidate is removed from Queue;
no production mouse patch, guest change or deployment is claimed by admission.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T435 S1; Ordinary Mode; source/device/input audit and bounded repair design. |
| Candidate Proposal | [Windows 1.01 EGA/InPort](../proposals/proposal-windows-101-ega-inport-runtime-001.md). |
| Admission And Approval | Owner: “准入任务win101 ega支持已经测试通过了 鼠标修复还有待执行 你准入 然后告诉我什么地方坏了 该怎么修”. Accept EGA as owner-tested baseline, audit pending mouse defects before production changes. |
| Objective | Identify current InPort register/timer/IRQ and host input gaps, compare accepted SoftPC correction, classify origin and design minimal repair/verification. |
| Non-goals | No EGA redesign, guest driver patch/reinstall, fake 8255/IRQ detection, Ignore workaround, CPU30, helper, new component, NTSRV/NTCON policy or 50Hz publication change. |
| Reference Baseline | T4349cf24314c/r037 APP434/RPC45/I/O25 installed ten images; original CCPU40. Owner EGA acceptance and original Mouse6.24/WIN100.BIN installation retained; old RPC42 diagnostic is evidence only, not current runtime proof. |
| Files And ABI Surface | Read mouse.c/quick_ev/PIC original owners and NTVDM project input bridge/guest adapter; finite source comparison with SoftPC accepted ee62ad01/9d102563. No wire change admitted. |
| Applicable Rules | AGENTS authorities, source policy/original guest immutability, matching SoftPC fix only for inherited host defects, mirror minimality/register, added adaptation fixes at NTVDM owner. |
| Verification | Source/hash/provenance ledger, actual call graph and IRQ/PIC/CPU-thread/quick-event lifecycle; classify prior real diagnostic separately from current source conclusions. Read-only inspection only in S1; test/build outputs below build/M0-T435/S1. |
| Expected Markers | Two separate gaps: finite IRQ burst versus real timer, and missing mouse_send input delivery; no claim that either alone proves the complete failure cause or fixes it. |
| Asset Needs | Existing original installation O:/Windows and accepted SoftPC code/history read-only; do not alter media or use other-project binaries as dependencies. Only Z: may be used in subsequent owned tests. |
| Reporting Requirements | Explain broken owner/boundary, source confidence, minimal diff, regression and unknown PIC/vector/reset/teardown contributions. Record independent Invalid handle exit report without attributing it to mouse. |
| Stop Conditions | No matching source correction; guest/host mutation outside scope; hardware patch expands into scheduler, snapshot or frontend-specific policy; unknown cause concealed by forced detection. |
| Exit Criteria | Reviewed concise source/input/IRQ plan, provenance and test entrypoint design; owner receives concrete repair explanation. Implementation needs sequential stage admission, not a claim from source comparison. |
| Original Owner Request | “准入任务win101 ega支持已经测试通过了 鼠标修复还有待执行 你准入 然后告诉我什么地方坏了 该怎么修”. |
| Similar-Issue Sweep | IRQ enable/disable, mode30/50/100/200Hz, HOLD/reset/cancellation, signed movement/buttons, hardware versus INT33 path, duplicate consumption and normal return. |

## Current Technical Baseline

T434 production9cf24314c, closure e4a2e72ab; APP0.0.434/RPC45/I/O25.
Its r037 ten-image package remains installed at O:/winnt/system32; original
CCPU40 and guest/mirror contracts are retained. S1 is not a new product package.

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

S1 audit/design is the only active stage. Do not widen to generic performance,
Windows 3.x or inherited host fixes without matching source. Remaining candidate
ordering is in Queue; preserve other-session planning edits.

## Recent Governance

T435 S1 is source audit/design only. Owner authorizes committing all pending
planning documents together; admission consumes only the Windows 1.01 head.
EGA is owner-accepted, mouse repair remains pending and no production code
or installed package is changed by this admission.

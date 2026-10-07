# Project Status

## Current Work

**Active: M0 T436 S1** (Ordinary Mode; Windows 3.1 non-WOW feasibility audit).
Owner closes T435 and directly admits the former queue-tail Windows 3.x
candidate, prioritizing Windows 3.1. [T435 closure](../history/m0-t435-win101-addon-closure.md)
records accepted installation/add-on delivery and preserved compatibility limits.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T436 S1; Ordinary Mode; bounded source/media/mode audit and design. |
| Candidate Proposal | [Windows 3.x non-WOW feasibility](../proposals/proposal-windows-3x-dos-runtime-001.md). |
| Admission And Approval | Owner: “收口，并将队尾windows 3.1的T任务直接提到开始，直接准入。” T435 is accepted and closed; admit this candidate next, with Windows 3.1 as the first target. |
| Objective | Establish available Windows 3.1 media/configuration identity, original launch and execution-mode requirements, current CCPU40/DOS/device boundaries, and a bounded test/design matrix before implementation. |
| Non-goals | No guest/core patches, mirror algorithm repair, second emulator, helper, host mutation or claim of Windows 3.x compatibility. Do not assume every version supports every mode. |
| Reference Baseline | Published T435 S2 APP0.0.435/RPC45/I/O25 ten images and matching assets/release manifest; accepted Windows 1.01 guest execution/mouse and S3 installation workflow. |
| Files And ABI Surface | Audit records under docs/etc; read-only original guest/source/reference inputs. Disposable manifests/probes under build/M0-T436/S1. No product/wire change admitted by S1. |
| Applicable Rules | AGENTS reading set, EXECUTION, architecture/coding/document rules, source policy and original guest immutability; inherited host defects only matching existing SoftPC correction, project defects separately designed. |
| Verification | Source-proven version/mode and dependency matrix; original media SHA/size/configuration manifest; map existing evidence and actual gaps. Documentation governance/link/diff checks. Any later launch probe must declare exact immutable inputs, bounded budget and pinned cleanup first. No drive mappings. |
| Expected Markers | Windows 3.1 standard/enhanced mode contracts and availability; distinguish guest kernel/USER/GDI from WOW; identify each dependency owner, supported/unknown boundary and proposed next S scope. |
| Asset Needs | Discover owner-supplied Windows 3.1 installation/media read-only, including O:/win31 if still available; no download/import or missing-media success assumption. Any comparison environment remains evidence only, never product dependency. |
| Reporting Requirements | Version/mode, inputs, original-source provenance, current capabilities, gap attribution, feasibility confidence and decisions requiring owner approval. |
| Stop Conditions | Missing/unproven media or provenance, requested guest mutation, architecture expansion, destructive original changes or unsupported claim: preserve and report; do not guess or silently implement. |
| Exit Criteria | Reviewed bounded audit/design with evidence-backed matrix, explicit missing prerequisites and next implementation/observation stages; commit/push. Audit is not runtime compatibility acceptance. |
| Original Owner Request | “收口，并将队尾windows 3.1的T任务直接提到开始，直接准入。” |
| Similar-Issue Sweep | DOS version/environment, mode-selection flags, A20/XMS/EMS/DPMI/DOSX, protected/V86 transitions, VGA/input, guest DLL search and distinction from host WOW. |

## Current Technical Baseline

T435 S2 r060 ten-image package remains published at O:/winnt/system32,
APP0.0.435/RPC45/I/O25. Its tests/publication/smoke are recorded in
[S2 evidence](../etc/evidence/m0-t435-s2-int33-driver.md).
assets/release/release-manifest.json matches all ten tested and deployed hashes.
Asset synchronization/rule delivery9e2b16170 is pushed; no new runtime package.

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

## Next Admission

S1 audits Windows 3.1 first, using the admitted proposal's version/mode boundary.
No runtime success, production fix or subsequent S admission is claimed yet.
All other unadmitted Queue candidates retain relative order.

## Recent Governance

Owner-required ten-image assets synchronization is recorded in EXECUTION;
9e2b16170 delivers matching published/tested assets. This admission is
documentation-only and does not require a new runtime build or publication.
